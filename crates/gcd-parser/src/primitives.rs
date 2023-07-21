use std::any::type_name;
use std::marker::PhantomData;
use gcd_utils::Endian;
use crate::{Action, Reader};
use crate::consume::Consume;
use crate::error::ParseError;
use crate::parse::FileCtx;

// Common Primitives

pub struct Todo<O> {
    _phantom: PhantomData<O>,
}

impl<O> Reader for Todo<O> {
    type Output = O;

    fn go(&self, _: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
        todo!()
    }
}

impl<O> Action for Todo<O> {
    type Output<'a> = O;

    fn go<'a>(&self, _: &'a FileCtx<'_>) -> Result<Self::Output<'a>, ParseError> {
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

impl<const N: usize> Reader for Magic<N> {
    type Output = ();
    
    fn go(&self, file: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
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

impl<I: Consume> Reader for Consumable<I> {
    type Output = I;
    
    fn go(&self, file: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
        file.consume(self.endian)
    }
}

pub fn u8() -> impl Reader<Output = u8> {
    Consumable { endian: Endian::Big, _phantom: PhantomData }
}

pub fn u16(endian: Endian) -> impl Reader<Output = u16> {
    Consumable { endian, _phantom: PhantomData }
}

pub fn u32(endian: Endian) -> impl Reader<Output = u32> {
    Consumable { endian, _phantom: PhantomData }
}

pub fn i32(endian: Endian) -> impl Reader<Output = i32> {
    Consumable { endian, _phantom: PhantomData }
}

pub fn f32(endian: Endian) -> impl Reader<Output = f32> {
    Consumable { endian, _phantom: PhantomData }
}

pub struct JumpRead<P, A> {
    pos: A,
    inner: P,
}

impl<P, A> Reader for JumpRead<P, A>
where
    P: Reader,
    A: for<'a> Action<Output<'a> = usize>,
{
    type Output = P::Output;

    fn go(&self, ctx: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
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

impl<T: 'static> Action for Memorized<T> {
    type Output<'a> = &'a T;

    fn go<'a>(&self, ctx: &'a FileCtx<'_>) -> Result<Self::Output<'a>, ParseError> {
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

impl<T: 'static> Action for Current<T> {
    type Output<'a> = &'a T;

    fn go<'a>(&self, ctx: &'a FileCtx<'_>) -> Result<Self::Output<'a>, ParseError> {
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
        impl<$head, $($tail,)*> Reader for Group<($head, $($tail,)*)>
        where
            $head: Reader,
            $($tail: Reader,)*
        {
            type Output = ($head::Output, $($tail::Output),*);
            
            #[allow(non_snake_case)]
            fn go(&self, ctx: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
                let ($head, $($tail,)*) = &self.inner;
                
                let out = (
                    $head.go(ctx)?,
                    $($tail.go(ctx)?,)*
                );
                
                Ok(out)
            }
        }
        
        impl<$head, $($tail,)*> Action for Group<($head, $($tail,)*)>
        where
            $head: Action,
            $($tail: Action,)*
        {
            type Output<'a> = ($head::Output<'a>, $($tail::Output<'a>),*);
            
            #[allow(non_snake_case)]
            fn go<'a>(&self, ctx: &'a FileCtx<'_>) -> Result<Self::Output<'a>, ParseError> {
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
