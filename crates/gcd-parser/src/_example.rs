use crate::primitives::{group, memorized, todo};
use crate::{Action, Reader};

fn foo() {
    /// Reader: A series of actions that may depend on each other in non-linear manner
    ///         Often, one will read a section, then use info from it to choose where to jump
    ///         and what to read at that location
    /// Action: Immutable behavior that pulls data out of previous steps to decide what to do next

    struct Header {
        pos1: usize,
        len1: usize,
        pos2: usize,
        len2: usize,
    }

    struct Section1 {}
    struct Section2 {}

    let header = todo::<Header>();
    let section1 = todo::<Section1>();
    let section2 = todo::<Section2>();

    // let file = header
    //     .memorize()
    //     .jump_and(
    //         memorized::<Header>().map(|h| h.pos1),
    //         section1.repeated()
    //             .exactly(memorized::<Header>().map(|h| h.len1))
    //             .memorize(),
    //     )
    //     .jump_and(
    //         memorized::<Header>().map(|h| h.pos2),
    //         section2.memorize(),
    //     )
    //     .retrieve::<(), _>(
    //         group((memorized::<Header>(), memorized::<Vec<Section1>>(), memorized::<Section2>())).map(|(h, sec1s, sec2)| todo!())
    //     );
}
