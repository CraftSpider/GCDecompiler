use std::fs::File;
use std::io;
use std::io::{BufReader, Seek, SeekFrom};
use std::path::Path;
use gcd_utils::multi_range::MultiRange;
use gcd_utils::Endian;
use crate::consume::Consume;
use crate::error::ParseError;

pub enum LintLevel {
    Ignore,
    Warn,
    Error,
}

pub struct ParseLints {
    on_unused: LintLevel,
    on_overlap: LintLevel,
}

impl Default for ParseLints {
    fn default() -> Self {
        ParseLints {
            on_unused: LintLevel::Ignore,
            on_overlap: LintLevel::Error,
        }
    }
}

pub struct ParseBuilder {
    lints: ParseLints,
}

impl ParseBuilder {
    pub fn finish<P: AsRef<Path>>(self, path: P) -> Result<ParseFile, io::Error> {
        let mut reader = BufReader::new(File::open(path)?);
        let len = reader.seek(SeekFrom::End(0))?;
        reader.seek(SeekFrom::Start(0))?;
        Ok(ParseFile {
            inner: reader,
            stream_pos: 0,
            stream_len: len as usize,
            lints: self.lints,
            used: MultiRange::new(),
        })
    }
}

impl Default for ParseBuilder {
    fn default() -> Self {
        ParseBuilder {
            lints: ParseLints::default(),
        }
    }
}

pub struct ParseFile {
    pub(super) inner: BufReader<File>,
    stream_len: usize,
    stream_pos: usize,
    lints: ParseLints,
    used: MultiRange<usize>,
}

impl ParseFile {
    pub fn build() -> ParseBuilder {
        ParseBuilder::default()
    }

    pub fn open<P: AsRef<Path>>(path: P) -> Result<ParseFile, io::Error> {
        Self::build().finish(path)
    }
    
    pub(super) fn cur_pos(&self) -> usize {
        self.stream_pos
    }

    pub(super) fn consume<T: Consume>(&mut self, endian: Endian) -> Result<T, ParseError> {
        // TODO: Cover lint - reading already read territory
        T::consume(self, endian)
    }

    pub(super) fn jump(&mut self, offset: usize) -> Result<(), ParseError> {
        // TODO: Cover lint - jumping into already read territory
        self.inner.seek(SeekFrom::Start(offset as u64))?;
        Ok(())
    }

    pub(super) fn finish(self) -> Result<(), ParseError> {
        if !self.used.covered(0..self.stream_len) {
            match self.lints.on_unused {
                LintLevel::Ignore => (),
                LintLevel::Warn => tracing::warn!("Not all bytes of input file consumed"),
                LintLevel::Error => return Err(ParseError::UnusedErr),
            }
        }
        Ok(())
    }
}
