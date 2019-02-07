
#include "at_tests"

#include "ppc/test_instructions.h"
#include "ppc/test_registers.h"
#include "ppc/test_symbols.h"

int main() {
    TEST_FILE(instructions)
    TEST_FILE(registers)
    TEST_FILE(symbols)
    
    return testing::run_tests("GCDecompiler") & 0b011;
}