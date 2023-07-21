use std::convert::Infallible;

pub struct Node<T, L>(T, L);

impl<T, L: TList> TList for Node<T, L> {
    type Item = T;
    type Next = L;

    fn item(&self) -> Option<&Self::Item> {
        Some(&self.0)
    }
    
    fn push<U>(self, item: U) -> Node<U, Self> {
        Node(item, self)
    }
    
    fn pop(self) -> (Option<Self::Item>, Self::Next) {
        (Some(self.0), self.1)
    }

    fn next(&self) -> &Self::Next {
        &self.1
    }
}

pub struct Null;

impl TList for Null {
    type Item = Infallible;
    type Next = Infallible;

    fn item(&self) -> Option<&Self::Item> {
        None
    }

    fn push<T>(self, item: T) -> Node<T, Self> {
        Node(item, self)
    }

    fn pop(self) -> (Option<Self::Item>, Self::Next) {
        panic!("Attempted to pop empty TList")
    }

    fn next(&self) -> &Self::Next {
        panic!("Atttempted to get next item of empty TList")
    }
}

pub trait TList: Sized {
    type Item;
    type Next: TList;
    
    fn item(&self) -> Option<&Self::Item>;
    
    fn push<T>(self, item: T) -> Node<T, Self>; 
    
    fn pop(self) -> (Option<Self::Item>, Self::Next);
    
    fn next(&self) -> &Self::Next;
}

impl TList for Infallible {
    type Item = Infallible;
    type Next = Infallible;

    fn item(&self) -> Option<&Self::Item> {
        match *self {}
    }

    fn push<T>(self, _: T) -> Node<T, Self> {
        match self {}
    }

    fn pop(self) -> (Option<Self::Item>, Self::Next) {
        match self {}
    }

    fn next(&self) -> &Self::Next {
        match *self {}
    }
}
