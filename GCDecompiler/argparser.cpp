//
// Created by Rune Tynan on 8/9/2018.
//

#include "logging.h"
#include "argparser.h"

ArgParser::ArgParser(const int &argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
    }
}
