//! A library for parsing strongly-contextual binary formats - think ELF files, things with lots of
//! offsets and order-dependent section parsing needed.
//! 
//! The two main traits are:
//! - [`Reader`], which is responsible for advancing the input and storing parsed values
//! - [`Action`], which is responsible for controlling readers through immutable access to stored
//!   values
//! 

mod _example;
pub mod error;
pub mod consume;
pub mod parse;
pub mod file;
pub mod combinator;
pub mod primitives;

use std::marker::PhantomData;
use gcd_utils::tlist::{Node, TList};
use crate::{
    error::ParseError,
    parse::FileCtx,
    file::ParseFile,
    combinator::*,
};

pub trait Reader {
    type Output;

    fn go(&self, ctx: &mut FileCtx<'_>) -> Result<Self::Output, ParseError>;

    fn parse(&self, mut file: ParseFile) -> Result<Self::Output, ParseError> {
        let mut ctx = FileCtx::new(&mut file);
        let out = self.go(&mut ctx)?;
        file.finish()?;
        Ok(out)
    }
    
    fn repeated(self) -> Repeated<Self, ()>
    where
        Self: Sized,
    {
        Repeated {
            inner: self,
            method: (),
        }
    }
    
    fn and<R>(
        self,
        second: R,
    ) -> And<Self, R>
    where
        Self: Sized,
        R: Reader,
    {
        And {
            first: self,
            second,
        }
    }
    
    fn jump_and<A, R>(
        self,
        pos: A,
        second: R,
    ) -> JumpAnd<Self, R, A>
    where
        Self: Sized,
        A: for<'a> Action<Output<'a> = usize>,
        R: Reader,
    {
        JumpAnd {
            first: self,
            second,
            pos,
        }
    }
    
    fn memorize(self) -> Memorize<Self>
    where
        Self: Sized,
    {
        Memorize { inner: self }
    }
    
    fn map<T, F>(self, func: F) -> Map<Self, F>
    where
        Self: Sized,
        F: Fn(Self::Output) -> T,
    {
        Map {
            inner: self,
            func,
        }
    }
    
    fn map_with<T, A, F>(self, action: A, func: F) -> MapWith<Self, A, F, T>
    where
        Self: Sized,
        A: Action,
        F: Fn(Self::Output, A::Output<'_>) -> T,
    {
        MapWith {
            inner: self,
            action,
            func,
            _phantom: PhantomData,
        }
    }
    
    fn retrieve<T, A>(
        self,
        action: A,
    ) -> Retrieve<Self, A, T>
    where
        Self: Sized,
        A: for<'a> Action<Output<'a> = T>,
    {
        Retrieve {
            inner: self,
            action,
            _phantom: PhantomData,
        }
    }
}

pub trait Action {
    type Output<'a>;
    
    fn go<'a>(&self, ctx: &'a FileCtx<'_>) -> Result<Self::Output<'a>, ParseError>;
    
    fn map<T, F>(self, func: F) -> Map<Self, F>
    where
        Self: Sized,
        F: Fn(Self::Output<'_>) -> T,
    {
        Map {
            inner: self,
            func,
        }
    }
    
    fn copied(self) -> Copied<Self>
    where
        Self: Sized,
    {
        Copied { inner: self }
    }
    
    fn cloned(self) -> Cloned<Self>
    where
        Self: Sized,
    {
        Cloned { inner: self }
    }
}

pub trait StaticAction<T>: for<'a> Action<Output<'a> = T> {}

impl<A, T> StaticAction<T> for A
where
    for<'a> A: Action<Output<'a> = T>
{}

impl Action for () {
    type Output<'a> = ();

    fn go<'a>(&self, _: &'a FileCtx<'_>) -> Result<Self::Output<'a>, ParseError> {
        Ok(())
    }
}
