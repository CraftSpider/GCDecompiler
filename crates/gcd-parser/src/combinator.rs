use std::marker::PhantomData;
use crate::error::ParseError;
use crate::parse::FileCtx;
use crate::{Action, Reader, StaticAction};

// Common combinators

pub struct Map<P, F> {
    pub(crate) inner: P,
    pub(crate) func: F,
}

impl<R, F, U> Reader for Map<R, F>
where
    R: Reader,
    F: Fn(R::Output) -> U,
{
    type Output = U;

    fn go(&self, ctx: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
        self.inner
            .go(ctx)
            .map(&self.func)
    }
}

impl<A, F, U> Action for Map<A, F>
where
    A: Action,
    F: Fn(A::Output<'_>) -> U,
{
    type Output<'a> = U;

    fn go<'a>(&self, ctx: &'a FileCtx<'_>) -> Result<Self::Output<'a>, ParseError> {
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

impl<R, A, F, T> Reader for MapWith<R, A, F, T>
where
    R: Reader,
    A: Action,
    F: for<'a> Fn(R::Output, A::Output<'a>) -> T,
{
    type Output = T;

    fn go(&self, ctx: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
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
    pub fn exactly<A>(self, action: A) -> Repeated<R, RepeatExact<A>>
    where
        A: for<'a> Action<Output<'a> = usize> + 'static
    {
        Repeated {
            inner: self.inner,
            method: RepeatExact(action),
        }
    }
    
    pub fn until<A>(self, action: A) -> Repeated<R, RepeatUntil<A>>
    where
        A: for<'a> Action<Output<'a> = bool> + 'static
    {
        Repeated {
            inner: self.inner,
            method: RepeatUntil(action),
        }
    }
    
    pub fn for_each<I, A>(self, action: A) -> Repeated<R, ForEach<A, I>>
    where
        A: StaticAction<I>,
        I: IntoIterator,
    {
        Repeated {
            inner: self.inner,
            method: ForEach(action, PhantomData),
        }
    }
}

impl<R, A> Reader for Repeated<R, RepeatExact<A>>
where
    R: Reader,
    A: for<'a> Action<Output<'a> = usize>,
{
    type Output = Vec<R::Output>;

    fn go(&self, ctx: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
        let num = self.method.0.go(ctx)?;
        (0..num)
            .map(|_| self.inner.go(ctx))
            .collect()
    }
}

impl<R, A> Reader for Repeated<R, RepeatUntil<A>>
where
    R: Reader,
    A: for<'a> Action<Output<'a> = bool>,
{
    type Output = Vec<R>;

    fn go(&self, _: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
        todo!()
    }
}

impl<R, A, I> Reader for Repeated<R, ForEach<A, I>>
where
    R: Reader,
    A: StaticAction<I>,
    I: IntoIterator,
    I::Item: 'static,
{
    type Output = Vec<R::Output>;

    fn go(&self, ctx: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
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

impl<R1, R2> Reader for And<R1, R2>
where
    R1: Reader,
    R2: Reader,
{
    type Output = R2::Output;

    fn go(&self, ctx: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
        self.first.go(ctx)?;
        self.second.go(ctx)
    }
}

pub struct JumpAnd<R1, R2, A> {
    pub(crate) first: R1,
    pub(crate) second: R2,
    pub(crate) pos: A,
}

impl<R1, R2, A> Reader for JumpAnd<R1, R2, A>
where
    R1: Reader,
    R2: Reader,
    A: for<'a> Action<Output<'a> = usize>,
{
    type Output = R2::Output;

    fn go(&self, ctx: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
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

impl<R, A, T: 'static> Reader for Retrieve<R, A, T>
where
    R: Reader,
    A: for<'a> Action<Output<'a> = T>,
{
    type Output = T;
    
    fn go<'a>(&self, ctx: &'a mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
        self.inner.go(ctx)?;
        let out = self.action.go(ctx)?;
        Ok(out)
    }
}

pub struct Memorize<R> {
    pub(crate) inner: R,
}

impl<R> Reader for Memorize<R>
where
    R: Reader,
    R::Output: 'static,
{
    type Output = ();

    fn go(&self, ctx: &mut FileCtx<'_>) -> Result<Self::Output, ParseError> {
        let val = self.inner.go(ctx)?;
        ctx.memorize(val)
    }
}

// Action combinators

pub struct Copied<A> {
    pub(crate) inner: A,
}

impl<A, T> Action for Copied<A>
where
    A: for<'a> Action<Output<'a> = &'a T>,
    T: Copy,
{
    type Output<'a> = T;
    
    fn go<'a>(&self, ctx: &'a FileCtx<'_>) -> Result<Self::Output<'a>, ParseError> {
        let out = self.inner.go(ctx)?;
        Ok(*out)
    }
}

pub struct Cloned<A> {
    pub(crate) inner: A,
}

impl<A, T> Action for Cloned<A>
where
    A: for<'a> Action<Output<'a> = &'a T>,
    T: Clone,
{
    type Output<'a> = T;

    fn go<'a>(&self, ctx: &'a FileCtx<'_>) -> Result<Self::Output<'a>, ParseError> {
        let out = self.inner.go(ctx)?;
        Ok(out.clone())
    }
}
