#pragma once

#include "at_tests"

class TestInstruction : public testing::AbstractTest {

public:
    
    void run();
    
    void test_build();
    void test_copy();
    void test_move();
    void test_code_name();
    void test_get_variables();
    void test_used_registers();
    void test_destination_registers();
    void test_source_registers();
    
};

void run_instructions_tests();
