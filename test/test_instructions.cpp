
#include "test_instructions.h"
#include "instruction.h"

void TestInstruction::run() {
    TEST_METHOD(test_build)
    TEST_METHOD(test_copy)
    TEST_METHOD(test_move)
    TEST_METHOD(test_code_name)
    TEST_METHOD(test_get_variables)
    TEST_METHOD(test_used_registers)
    TEST_METHOD(test_destination_registers)
    TEST_METHOD(test_source_registers)
}

void TestInstruction::test_build() {
    // TODO
}

void TestInstruction::test_copy() {
    // TODO
}

void TestInstruction::test_move() {
    // TODO
}

void TestInstruction::test_code_name() {
    // TODO
}

void TestInstruction::test_get_variables() {
    // TODO
}

void TestInstruction::test_used_registers() {
    // TODO
}

void TestInstruction::test_destination_registers() {
    // TODO
}

void TestInstruction::test_source_registers() {
    // TODO
}

void test_condition() {
    // TODO
}

void run_instructions_tests() {
    TEST_CLASS(TestInstruction)
    TEST(test_condition)
}
