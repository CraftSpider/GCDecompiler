use std::collections::BTreeMap;
use std::ops::{Add, Range};

pub trait RangeNum: Copy + Ord + Add<Output = Self> {
    const ZERO: Self;
    const ONE: Self;
    
    fn saturating_sub(self, other: Self) -> Self;
}

impl RangeNum for usize {
    const ZERO: Self = 0;
    const ONE: Self = 1;

    fn saturating_sub(self, other: Self) -> Self {
        usize::saturating_sub(self, other)
    }
}

pub trait Coverable<T> {
    fn is_covered(self, ranges: &BTreeMap<T, T>) -> bool;
}

impl<T: RangeNum> Coverable<T> for T {
    fn is_covered(self, ranges: &BTreeMap<T, T>) -> bool{
        for (_, end) in ranges.range(..=self) {
            if self < *end {
                return true;
            }
        }
        false
    }
}

impl Coverable<usize> for Range<usize> {
    fn is_covered(self, ranges: &BTreeMap<usize, usize>) -> bool {
        // TODO
        false
    }
} 

#[derive(Debug)]
pub struct MultiRange<T> {
    // Pairs of start/end sets in the range
    // These should always be disjoint of each other - any time ranges touch, they merge into one
    pairs: BTreeMap<T, T>,
}

impl<T: RangeNum> MultiRange<T> {
    pub const fn new() -> MultiRange<T> {
        MultiRange { pairs: BTreeMap::new() }
    }

    pub fn covered<U: Coverable<T>>(&self, value: U) -> bool {
        U::is_covered(value, &self.pairs)
    }

    // CAREFUL: This is inclusive on end for internal use
    fn covering_range(&self, value: T) -> Option<(T, T)> {
        for (&start, &end) in self.pairs.range(..=value) {
            if value <= end {
                return Some((start, end))
            }
        }
        None
    }

    // CAREFUL: This is inclusive on end for internal use
    fn covering_range_mut(&mut self, value: T) -> Option<(&T, &mut T)> {
        for out in self.pairs.range_mut(..=value) {
            if value <= *out.1 {
                return Some(out)
            }
        }
        None
    }

    pub fn insert(&mut self, range: Range<T>) {
        if range.is_empty() {
            return;
        }
        
        let (rstart, rend) = if let Some((&start, end)) = self.covering_range_mut(range.start) {
            *end = T::max(*end, range.end);
            (start + T::ONE, *end)
        } else if let Some((start, end)) = self.covering_range(range.end) {
            if range.start < start {
                self.pairs.insert(range.start, T::max(range.end, end));
            }
            (T::min(range.start, start) + T::ONE, end)
        } else {
            self.pairs.insert(range.start, range.end);
            ((range.start + T::ONE), range.end)
        };

        let to_remove = self.pairs.range(rstart..=rend)
            .map(|(key, _)| *key)
            .collect::<Vec<_>>();

        to_remove.into_iter().for_each(|key| _ = self.pairs.remove(&key));
    }

    pub fn extend(&mut self, start: T, len: T) {
        self.insert(start..(start+len))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_covered() {
        let mut mr = MultiRange::new();
        
        mr.insert(1..5);
        
        println!("{:?}", mr);

        assert!(!mr.covered(0));
        assert!(mr.covered(1));
        assert!(mr.covered(4));
        assert!(!mr.covered(5));
    }
    
    /// ```no_compile
    /// - Given the range:
    /// |.....*----*.....|
    /// - Cases where start is covered
    /// |.....*--*.......|
    /// |.....*------*...|
    /// |........*----*..|
    /// ```
    #[test]
    fn test_add_start() {
        let mut mr = MultiRange::new();
        mr.insert(5..10);
        
        mr.insert(5..6);
        assert_eq!(mr.pairs, BTreeMap::from([(5, 10)]));
        
        mr.insert(6..7);
        assert_eq!(mr.pairs, BTreeMap::from([(5, 10)]));
        
        mr.insert(5..13);
        assert_eq!(mr.pairs, BTreeMap::from([(5, 13)]));
        
        mr.insert(10..15);
        assert_eq!(mr.pairs, BTreeMap::from([(5, 15)]));
        
        mr.insert(20..25);
        
        mr.insert(10..30);
        assert_eq!(mr.pairs, BTreeMap::from([(5, 30)]));
    }
    
    /// ```no_compile
    /// - Given the range:
    /// |.....*----*.....|
    /// - Cases where end is covered
    /// |..*-------*.....|
    /// |......*--*......|
    /// | *----*.........|
    /// ```
    #[test]
    fn test_add_end() {
        let mut mr = MultiRange::new();
        mr.insert(10..15);

        mr.insert(10..14);
        assert_eq!(mr.pairs, BTreeMap::from([(10, 15)]));
        
        mr.insert(11..14);
        assert_eq!(mr.pairs, BTreeMap::from([(10, 15)]));

        mr.insert(9..15);
        assert_eq!(mr.pairs, BTreeMap::from([(9, 15)]));

        mr.insert(7..11);
        assert_eq!(mr.pairs, BTreeMap::from([(7, 15)]));
        
        mr.insert(5..6);
        
        mr.insert(2..15);
        assert_eq!(mr.pairs, BTreeMap::from([(2, 15)]));
    }
    
    /// ```no_compile
    /// - Given the range:
    /// |.....*----*.....|
    /// - Cases where neither are covered
    /// |.*--*...........|
    /// |............*--*|
    /// |..*----------*..|
    /// ```
    #[test]
    fn test_add_disjoint() {
        let mut mr = MultiRange::new();
        mr.insert(5..10);
        
        mr.insert(1..2);
        assert_eq!(mr.pairs, BTreeMap::from([(1, 2), (5, 10)]));
        
        mr.insert(11..15);
        assert_eq!(mr.pairs, BTreeMap::from([(1, 2), (5, 10), (11, 15)]));
        
        mr.insert(0..20);
        assert_eq!(mr.pairs, BTreeMap::from([(0, 20)]));
    }
}
