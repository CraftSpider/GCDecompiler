
#include "at_tests"

#include "test_instructions.h"

int main(int argc, char **argv) {
    TEST_FILE(instructions)
    
    testing::run_tests("GCDecompiler");
}