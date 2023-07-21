use std::any::{Any, type_name, TypeId};
use std::borrow::Borrow;
use std::collections::HashSet;
use std::hash::{Hash, Hasher};
use gcd_utils::Endian;
use crate::consume::Consume;
use crate::error::ParseError;
use crate::file::ParseFile;

pub enum Stored {
    Taken(TypeId),
    Value(TypeId, Box<dyn Any>),
}

impl PartialEq for Stored {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (Stored::Taken(lid), Stored::Taken(rid)) => lid == rid,
            (Stored::Value(lid, _), Stored::Value(rid, _)) => lid == rid,
            _ => false,
        }
    }
}

impl Eq for Stored {}

impl Hash for Stored {
    fn hash<H: Hasher>(&self, state: &mut H) {
        match self {
            Stored::Taken(id) => id.hash(state),
            Stored::Value(id, _) => id.hash(state),
        }
    }
}

impl Borrow<TypeId> for Stored {
    fn borrow(&self) -> &TypeId {
        match self {
            Stored::Taken(id) => id,
            Stored::Value(id, _) => id,
        }
    }
}

pub struct FileCtx<'a> {
    file: &'a mut ParseFile,
    memorized: HashSet<Stored>,
    current: Option<Box<dyn Any>>,
}

impl<'a> FileCtx<'a> {
    pub(crate) fn new(file: &'a mut ParseFile) -> FileCtx<'a> {
        FileCtx { file, memorized: HashSet::new(), current: None }
    }
    
    pub(crate) fn consume<T: Consume>(&mut self, endian: Endian) -> Result<T, ParseError> {
        self.file.consume::<T>(endian)
    }

    pub(crate) fn mark(&self) -> usize {
        self.file.cur_pos()
    }

    pub(crate) fn jump(&mut self, pos: usize) -> Result<(), ParseError> {
        self.file.jump(pos)
    }
    
    pub(crate) fn memorize<T: Any>(&mut self, val: T) -> Result<(), ParseError> {
        match self.get_memorized::<T>() {
            Ok(_) => Err(ParseError::AlreadyMemorized(type_name::<T>())),
            Err(ParseError::NotMemorized(_)) => {
                self.memorized.insert(Stored::Value(TypeId::of::<T>(), Box::new(val)));
                Ok(())
            }
            Err(e) => Err(e),
        }
    }
    
    pub(crate) fn set_current<T: 'static>(&mut self, val: T) {
        self.current = Some(Box::new(val));
    }
    
    pub(crate) fn get_memorized<T: Any>(&self) -> Result<&T, ParseError> {
        self.memorized
            .get(&TypeId::of::<T>())
            .ok_or(ParseError::NotMemorized(type_name::<T>()))
            .and_then(|a| match a {
                Stored::Taken(_) => Err(ParseError::TakenMemorized(type_name::<T>())),
                Stored::Value(_, val) => Ok(val.downcast_ref().unwrap()),
            })
    }
    
    pub(crate) fn get_current<T: 'static>(&self) -> Option<&T> {
        self.current
            .as_ref()
            .and_then(|b| b.downcast_ref::<T>())
    }
}

