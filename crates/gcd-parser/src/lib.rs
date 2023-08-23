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
use gcd_utils::tlist::{Node, Null, TList};
use crate::{
    error::ParseError,
    parse::FileCtx,
    file::ParseFile,
    combinator::*,
};

pub trait Reader<L: TList> {
    type Output;

    fn go(&self, ctx: &mut FileCtx<'_, L>) -> Result<Self::Output, ParseError>;

    fn parse(&self, mut file: ParseFile) -> Result<<Self as Reader<Null>>::Output, ParseError> 
    where
        Self: Reader<Null>,
    {
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
        R: Reader<L>,
    {
        And {
            first: self,
            second,
        }
    }
    
    fn jump_and<'a, A, R>(
        self,
        pos: A,
        second: R,
    ) -> JumpAnd<Self, R, A>
    where
        Self: Sized,
        A: Action<'a, L, Output = usize>,
        R: Reader<L>,
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
    
    fn map_with<'a, T, A, F>(self, action: A, func: F) -> MapWith<Self, A, F, T>
    where
        Self: Sized,
        A: Action<'a, L>,
        F: Fn(Self::Output, A::Output) -> T,
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
        A: for<'a> Action<'a, L, Output = T>,
    {
        Retrieve {
            inner: self,
            action,
            _phantom: PhantomData,
        }
    }
}

pub trait Action<'a, L: TList> {
    type Output;
    
    fn go(&self, ctx: &'a FileCtx<'_, L>) -> Result<Self::Output, ParseError>;
    
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

impl<'a, L: TList> Action<'a, L> for () {
    type Output = ();

    fn go(&self, _: &'a FileCtx<'_, L>) -> Result<Self::Output, ParseError> {
        Ok(())
    }
}
