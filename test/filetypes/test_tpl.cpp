
#include <at_tests>

#include "datatypes/color.h"
#include "filetypes/tpl.h"
#include "filetypes/png.h"
#include "test_tpl.h"

using types::Color;

Color** make_color_block(uint w, uint h) {
    Color** data = new Color*[h];
    for (uint i = 0; i < h; ++i) {
        data[i] = new Color[w];
    }
    return data;
}

void fill_color(Color** data, uchar* block, uint w, uint h, Color(func)(const uchar*, uchar)) {
    for (uint i = 0; i < h; ++i) {
        for (uint j = 0; j < w; ++j) {
            data[i][j] = func(block, i * h + j);
        }
    }
}

void TestBlockParsers::test_parse_i4() {
    uchar block[] = {0x01, 0x23, 0x45, 0x67,
                     0x89, 0xAB, 0xCD, 0xEF,
                     0x01, 0x23, 0x45, 0x67,
                     0x89, 0xAB, 0xCD, 0xEF,
                     0x01, 0x23, 0xFE, 0xDC,
                     0x45, 0x67, 0xBA, 0x98,
                     0x89, 0xAB, 0x76, 0x54,
                     0xCD, 0xEF, 0x32, 0x10};
    
    Color** data = make_color_block(8, 8);
    fill_color(data, block, 8, 8, types::parse_i4);
    
    types::PNG png = types::PNG(types::Image(8, 8, data));
    png.save("./test_i4.png");
}

void TestBlockParsers::test_parse_i8() {
    throw testing::skip_test();
}

void TestBlockParsers::test_parse_rgb565() {
    uchar block[] = {
            0xF8, 0x00, 0x07, 0xE0, 0x00, 0x1F, 0x00, 0x00,
            0xF8, 0x00, 0x07, 0xE0, 0x00, 0x1F, 0x00, 0x00,
            0xFF, 0xE0, 0x07, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF,
            0xFF, 0xE0, 0x07, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF
            
    };
    
    Color** data = make_color_block(4, 4);
    fill_color(data, block, 4, 4, types::parse_rgb565);
    
    types::PNG png = types::PNG(types::Image(4, 4, data));
    png.save("./test_rgb565.png");
}

void TestBlockParsers::test_parse_rgb5A3() {
    throw testing::skip_test();
}

void TestBlockParsers::test_parse_cmpr() {
    uchar block[] = {
            0xF8, 0x00, 0x07, 0xE0, 0x0A, 0x0A, 0xF5, 0xF5,
            0x07, 0xE0, 0x00, 0x1F, 0x0A, 0x0A, 0xF5, 0xF5,
            0xF8, 0x00, 0x00, 0x1F, 0x5F, 0x5F, 0xA0, 0xA0,
            0x00, 0x00, 0xFF, 0xFF, 0x0A, 0x0A, 0x5F, 0x5F,
    };
    
    Color** data = make_color_block(8, 8);
    fill_color(data, block, 8, 8, types::parse_cmpr);
    
    types::PNG png = types::PNG(types::Image(8, 8, data));
    png.save("./test_cmpr.png");
}

void TestBlockParsers::run() {
    TEST_METHOD(test_parse_i4)
    TEST_METHOD(test_parse_i8)
    TEST_METHOD(test_parse_rgb565)
    TEST_METHOD(test_parse_rgb5A3)
    TEST_METHOD(test_parse_cmpr)
}

void run_tpl_tests() {
    TEST(TestBlockParsers())
}
