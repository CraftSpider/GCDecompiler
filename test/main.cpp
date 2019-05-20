
#include "at_tests"

#include "datatypes/test_color.h"
#include "filetypes/test_png.h"
#include "filetypes/test_tpl.h"
#include "ppc/test_instructions.h"
#include "ppc/test_registers.h"
#include "ppc/test_symbols.h"

int main(int argc, char** argv) {
    testing::setup_tests(argc, argv);
    
    TEST_FILE(color)
    
    TEST_FILE(png)
    TEST_FILE(tpl)
    
    TEST_FILE(instructions)
    TEST_FILE(registers)
    TEST_FILE(symbols)
    
    return testing::run_tests("GCDecompiler") & 0b011;
}