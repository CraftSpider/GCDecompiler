#pragma once

class TestBlockParsers : public testing::AbstractTest {

protected:
    
    void test_parse_i4();
    void test_parse_i8();
    void test_parse_rgb565();
    void test_parse_rgb5A3();
    void test_parse_cmpr();
    
public:
    
    void run();
    
};

void run_tpl_tests();
