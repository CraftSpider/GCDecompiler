
#include "at_tests"
#include "test_color.h"
#include "datatypes/color.h"

void test_constructor() {
    types::Color color = types::Color();
    
    ASSERT(color.R == 0);
    ASSERT(color.G == 0);
    ASSERT(color.B == 0);
    ASSERT(color.A == 255);
    
    types::Color colorRGB = types::Color(100, 150, 200);
    
    ASSERT(colorRGB.R == 100);
    ASSERT(colorRGB.G == 150);
    ASSERT(colorRGB.B == 200);
    ASSERT(colorRGB.A == 255);
    
    types::Color colorRGBA = types::Color(100, 110, 120, 130);
    
    ASSERT(colorRGBA.R == 100);
    ASSERT(colorRGBA.G == 110);
    ASSERT(colorRGBA.B == 120);
    ASSERT(colorRGBA.A == 130);
}

void test_to_int() {
    types::Color color = types::Color();
    
    ASSERT(color.to_int() == 0x000000FF);
    
    color = types::Color(100, 150, 200);
    
    ASSERT(color.to_int() == 0x6496C8FF);
    
    color = types::Color(110, 120, 130, 140);
    
    ASSERT(color.to_int() == 0x6E78828C);
}

void test_lerp() {
    throw testing::skip_test();
}

void run_color_tests() {
    TEST(test_constructor)
    TEST(test_to_int)
    TEST(test_lerp)
}
