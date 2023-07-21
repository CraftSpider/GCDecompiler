use std::io::Read;
use crate::error::ParseError;
use crate::file::ParseFile;
use gcd_utils::Endian;

pub trait Consume: Sized {
    fn consume(file: &mut ParseFile, endian: Endian) -> Result<Self, ParseError>;
}

impl<const N: usize> Consume for [u8; N] {
    fn consume(file: &mut ParseFile, _: Endian) -> Result<Self, ParseError> {
        let mut out = [0; N];
        file.inner.read_exact(&mut out)?;
        Ok(out)
    }
}

macro_rules! impl_int {
    ($ty:ty) => {
        impl Consume for $ty {
            fn consume(file: &mut ParseFile, endian: Endian) -> Result<Self, ParseError> {
                let f = match endian {
                    Endian::Big => <$ty>::from_be_bytes,
                    Endian::Little => <$ty>::from_le_bytes,
                };
                
                <[u8; { core::mem::size_of::<$ty>() }]>::consume(file, endian).map(f)
            }
        }
    }
}

impl_int!(u8);
impl_int!(u16);
impl_int!(u32);
impl_int!(u64);

impl_int!(i8);
impl_int!(i16);
impl_int!(i32);
impl_int!(i64);

impl_int!(f32);
impl_int!(f64);
