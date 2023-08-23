use std::any::type_name;
use std::marker::PhantomData;
use gcd_utils::Endian;
use gcd_utils::tlist::{Null, TList};
use crate::{Action, Reader};
use crate::consume::Consume;
use crate::error::ParseError;
use crate::parse::FileCtx;

// Common Primitives

pub struct Todo<O> {
    _phantom: PhantomData<O>,
}

impl<O> Reader<Null> for Todo<O> {
    type Output = O;

    fn go(&self, _: &mut FileCtx<'_, Null>) -> Result<Self::Output, ParseError> {
        todo!()
    }
}

impl<'a, O, L: TList> Action<'a, L> for Todo<O> {
    type Output = O;

    fn go(&self, _: &'a FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        todo!()
    }
}

pub fn todo<O>() -> Todo<O> {
    Todo { _phantom: PhantomData }
}

// Reader primitives

pub struct Magic<const N: usize> {
    bytes: [u8; N],
}

impl<const N: usize, L: TList> Reader<L> for Magic<N> {
    type Output = ();
    
    fn go(&self, file: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        let mark = file.mark();
        let bytes = file.consume::<[u8; N]>(Endian::Big)?;
        if bytes == self.bytes {
            Ok(())
        } else {
            file.jump(mark)?;
            Err(ParseError::Custom(format!("Expected bytes {:?}, got {:?}", self.bytes, bytes)))
        }
    }
}

pub fn magic<const N: usize>(bytes: [u8; N]) -> Magic<N> {
    Magic { bytes }
}

struct Consumable<I: Consume> {
    endian: Endian,
    _phantom: PhantomData<I>,
}

impl<I: Consume, L: TList> Reader<L> for Consumable<I> {
    type Output = I;
    
    fn go(&self, file: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        file.consume(self.endian)
    }
}

pub fn u8() -> impl Reader<Null, Output = u8> {
    Consumable { endian: Endian::Big, _phantom: PhantomData }
}

pub fn u16(endian: Endian) -> impl Reader<Null, Output = u16> {
    Consumable { endian, _phantom: PhantomData }
}

pub fn u32(endian: Endian) -> impl Reader<Null, Output = u32> {
    Consumable { endian, _phantom: PhantomData }
}

pub fn i32(endian: Endian) -> impl Reader<Null, Output = i32> {
    Consumable { endian, _phantom: PhantomData }
}

pub fn f32(endian: Endian) -> impl Reader<Null, Output = f32> {
    Consumable { endian, _phantom: PhantomData }
}

pub struct JumpRead<P, A> {
    pos: A,
    inner: P,
}

impl<P, A, L: TList> Reader<L> for JumpRead<P, A>
where
    P: Reader<L>,
    A: for<'a> Action<'a, L, Output = usize>,
{
    type Output = P::Output;

    fn go(&self, ctx: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        ctx.jump(self.pos.go(ctx)?)?;
        let out = self.inner.go(ctx)?;
        Ok(out)
    }
}

pub fn jump_read<A, P>(pos: A, inner: P) -> JumpRead<P, A> {
    JumpRead {
        pos,
        inner,
    }
}

// Action primitives

pub struct Memorized<T> {
    _phantom: PhantomData<T>,
}

impl<'a, T, L: TList> Action<'a, L> for Memorized<T>
where
    T: 'a,
{
    type Output = &'a T;

    fn go(&self, ctx: &'a FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        match ctx.get_memorized::<T>() {
            Ok(val) => Ok(val),
            Err(e) => Err(e)
        }
    }
}

pub fn memorized<T>() -> Memorized<T> {
    Memorized { _phantom: PhantomData }
}

pub struct Current<T> {
    _phantom: PhantomData<T>,
}

impl<'a, T, L: TList> Action<'a, L> for Current<T>
where
    T: 'a,
{
    type Output = &'a T;

    fn go(&self, ctx: &'a FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        match ctx.get_current() {
            Some(val) => Ok(val),
            None => Err(ParseError::NotCurrent(type_name::<T>()))
        }
    }
}

pub fn current<T>() -> Current<T> {
    Current { _phantom: PhantomData }
}

pub struct Group<T> {
    inner: T,
}

macro_rules! group_action {
    ($head:ident $headlt:lifetime, $($tail:ident $taillt:lifetime),* $(,)?) => {
        impl<Ls: TList, $head, $($tail,)*> Reader<Ls> for Group<($head, $($tail,)*)>
        where
            $head: Reader<Ls>,
            $($tail: Reader<Ls>,)*
        {
            type Output = ($head::Output, $($tail::Output),*);
            
            #[allow(non_snake_case)]
            fn go(&self, ctx: &mut FileCtx<'_, Ls>) -> Result<Self::Output, ParseError> {
                let ($head, $($tail,)*) = &self.inner;
                
                let out = (
                    $head.go(ctx)?,
                    $($tail.go(ctx)?,)*
                );
                
                Ok(out)
            }
        }
        
        impl<'a, Ls: TList, $head, $($tail,)*> Action<'a, Ls> for Group<($head, $($tail,)*)>
        where
            $head: Action<'a, Ls>,
            $($tail: Action<'a, Ls>,)*
        {
            type Output = ($head::Output, $($tail::Output),*);
            
            #[allow(non_snake_case)]
            fn go(&self, ctx: &'a FileCtx<'_, Ls>) -> Result<Self::Output, ParseError> {
                let ($head, $($tail,)*) = &self.inner;
                
                let out = (
                    $head.go(ctx)?,
                    $($tail.go(ctx)?,)*
                );
                
                Ok(out)
            }
        }
        
        group_action!($($tail $taillt,)*);
    };
    () => {}
}

group_action!(A 'a, B 'b, C 'c, D 'd, E 'e, F 'f, G 'g, H 'h, I 'i, J 'j, K 'k, L 'l, M 'm);

pub fn group<T>(group: T) -> Group<T> {
    Group { inner: group }
}
