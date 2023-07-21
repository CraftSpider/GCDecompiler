use std::error::Error;
use std::{fmt, io};
use std::fmt::Formatter;

#[derive(Debug)]
pub enum ParseError {
    Cause(/*Backtrace, */Box<dyn Error>),
    UnusedErr,
    OverlapErr,
    AlreadyMemorized(&'static str),
    NotMemorized(&'static str),
    TakenMemorized(&'static str),
    NotCurrent(&'static str),
    Custom(String),
}

impl From<io::Error> for ParseError {
    #[track_caller]
    fn from(err: io::Error) -> Self {
        ParseError::Cause(/*Backtrace::capture(), */Box::new(err))
    }
}

impl fmt::Display for ParseError {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        todo!()
    }
}

impl Error for ParseError {}
