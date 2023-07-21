
# File Parser

Tool for highly-contextual file parsing

## Design

- Reader
  - Consumes data from the input
  - Mutates input state (current position, etc)
- Action
  - Retrieves state
  - Immutable, often fed as input to a reader

Problem - need to better handle nested loop state, enumeration, etc.

Just 'memorized' and 'current' isn't enough - current would get overwritten, plus, would be
nice to be able to do non-`'static` items as current loop value

Type-level list has issues - lifetimes become ugly quick, and it's hard to implement
since it means `Action` and `Reader` become generic over their contained list.
