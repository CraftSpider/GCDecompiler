use std::marker::PhantomData;
use gcd_utils::tlist::TList;
use crate::error::ParseError;
use crate::parse::FileCtx;
use crate::{Action, Reader};

// Common combinators

pub struct Map<P, F> {
    pub(crate) inner: P,
    pub(crate) func: F,
}

impl<R, F, U, L: TList> Reader<L> for Map<R, F>
where
    R: Reader<L>,
    F: Fn(R::Output) -> U,
{
    type Output = U;

    fn go(&self, ctx: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        self.inner
            .go(ctx)
            .map(&self.func)
    }
}

impl<'a, A, F, U, L: TList> Action<'a, L> for Map<A, F>
where
    A: Action<'a, L>,
    F: Fn(A::Output) -> U,
{
    type Output = U;

    fn go(&self, ctx: &'a FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        self.inner
            .go(ctx)
            .map(&self.func)
    }
}

// Reader combinators

pub struct MapWith<R, A, F, T> {
    pub(crate) inner: R,
    pub(crate) action: A,
    pub(crate) func: F,
    pub(crate) _phantom: PhantomData<T>,
}

impl<'a, R, A, F, T, L: TList> Reader<L> for MapWith<R, A, F, T>
where
    R: Reader<L>,
    A: Action<'a, L>,
    F: Fn(R::Output, A::Output) -> T,
{
    type Output = T;

    fn go(&self, ctx: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        let r = self.inner.go(ctx)?;
        let a = self.action.go(ctx)?;
        
        Ok((self.func)(r, a))
    }
}

pub struct RepeatExact<A>(A);

pub struct RepeatUntil<A>(A);

pub struct ForEach<A, I>(A, PhantomData<I>);

pub struct Repeated<R, A> {
    pub(crate) inner: R,
    pub(crate) method: A,
}

impl<R> Repeated<R, ()> {
    pub fn exactly<A, L: TList>(self, action: A) -> Repeated<R, RepeatExact<A>>
    where
        A: for<'a> Action<'a, L, Output = usize> + 'static
    {
        Repeated {
            inner: self.inner,
            method: RepeatExact(action),
        }
    }
    
    pub fn until<A, L: TList>(self, action: A) -> Repeated<R, RepeatUntil<A>>
    where
        A: for<'a> Action<'a, L, Output = bool> + 'static
    {
        Repeated {
            inner: self.inner,
            method: RepeatUntil(action),
        }
    }
    
    pub fn for_each<'a, I, A, L: TList>(self, action: A) -> Repeated<R, ForEach<A, I>>
    where
        A: Action<'a, L, Output = I>,
        I: IntoIterator + 'a,
    {
        Repeated {
            inner: self.inner,
            method: ForEach(action, PhantomData),
        }
    }
}

impl<'a, R, A, L: TList> Reader<L> for Repeated<R, RepeatExact<A>>
where
    R: Reader<L>,
    A: Action<'a, L, Output = usize>,
{
    type Output = Vec<R::Output>;

    fn go(&self, ctx: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        let num = self.method.0.go(ctx)?;
        (0..num)
            .map(|_| self.inner.go(ctx))
            .collect()
    }
}

impl<R, A, L: TList> Reader<L> for Repeated<R, RepeatUntil<A>>
where
    R: Reader<L>,
    A: for<'a> Action<'a, L, Output = bool>,
{
    type Output = Vec<R>;

    fn go(&self, _: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        todo!()
    }
}

impl<'a, R, A, I, L: TList> Reader<L> for Repeated<R, ForEach<A, I>>
where
    R: Reader<L>,
    A: Action<'a, L, Output = I>,
    I: IntoIterator + 'a,
    I::Item: 'a,
{
    type Output = Vec<R::Output>;

    fn go(&self, ctx: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        let num = self.method.0.go(ctx)?;
        num.into_iter()
            .map(|val| {
                ctx.set_current(val);
                self.inner.go(ctx)
            })
            .collect()
    }
}

pub struct And<R1, R2> {
    pub(crate) first: R1,
    pub(crate) second: R2,
}

impl<R1, R2, L: TList> Reader<L> for And<R1, R2>
where
    R1: Reader<L>,
    R2: Reader<L>,
{
    type Output = R2::Output;

    fn go(&self, ctx: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        self.first.go(ctx)?;
        self.second.go(ctx)
    }
}

pub struct JumpAnd<R1, R2, A> {
    pub(crate) first: R1,
    pub(crate) second: R2,
    pub(crate) pos: A,
}

impl<'a, R1, R2, A, L: TList> Reader<L> for JumpAnd<R1, R2, A>
where
    R1: Reader<L>,
    R2: Reader<L>,
    A: Action<'a, L, Output = usize>,
{
    type Output = R2::Output;

    fn go(&self, ctx: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        self.first.go(ctx)?;
        let pos = self.pos.go(ctx)?;
        ctx.jump(pos)?;
        self.second.go(ctx)
    }
}

pub struct Retrieve<R, A, T> {
    pub(crate) inner: R,
    pub(crate) action: A,
    pub(crate) _phantom: PhantomData<T>,
}

impl<R, A, T: 'static, L: TList> Reader<L> for Retrieve<R, A, T>
where
    R: Reader<L>,
    A: Action<'static, L, Output = T>,
{
    type Output = T;
    
    fn go<'a>(&self, ctx: &'a mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        self.inner.go(ctx)?;
        let out = self.action.go(ctx)?;
        Ok(out)
    }
}

pub struct Memorize<R> {
    pub(crate) inner: R,
}

impl<R, L: TList> Reader<L> for Memorize<R>
where
    R: Reader<L>,
    R::Output: 'static,
{
    type Output = ();

    fn go(&self, ctx: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        let val = self.inner.go(ctx)?;
        ctx.memorize(val)
    }
}

// Action combinators

pub struct Copied<A> {
    pub(crate) inner: A,
}

impl<'a, A, T, L: TList> Action<'a, L> for Copied<A>
where
    A: Action<'a, L, Output = &'a T>,
    T: Copy + 'a,
{
    type Output = T;
    
    fn go(&self, ctx: &'a FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        let out = self.inner.go(ctx)?;
        Ok(*out)
    }
}

pub struct Cloned<A> {
    pub(crate) inner: A,
}

impl<'a, A, T, L: TList> Action<'a, L> for Cloned<A>
where
    A: Action<'a, L, Output = &'a T>,
    T: Clone + 'a,
{
    type Output = T;

    fn go(&self, ctx: &'a FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        let out = self.inner.go(ctx)?;
        Ok(out.clone())
    }
}
