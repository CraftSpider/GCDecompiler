
#include "test_instructions.h"
#include "ppc/instruction.h"

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
    throw testing::skip_test();
}

void TestInstruction::test_copy() {
    throw testing::skip_test();
}

void TestInstruction::test_move() {
    throw testing::skip_test();
}

void TestInstruction::test_code_name() {
    throw testing::skip_test();
}

void TestInstruction::test_get_variables() {
    throw testing::skip_test();
}

void TestInstruction::test_used_registers() {
    throw testing::skip_test();
}

void TestInstruction::test_destination_registers() {
    throw testing::skip_test();
}

void TestInstruction::test_source_registers() {
    throw testing::skip_test();
}

void test_condition() {
    throw testing::skip_test();
}

void run_instructions_tests() {
    TEST_CLASS(TestInstruction)
    TEST(test_condition)
}
