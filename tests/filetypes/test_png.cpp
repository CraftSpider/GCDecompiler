
#include <at_tests>

#include "filetypes/png.h"
#include "test_png.h"

using types::Color;

void test_png_save() {
    Color** data = new Color*[8];
    for (int i = 0; i < 8; ++i) {
        data[i] = new Color[12]();
        if (i < 4) {
            data[i][0] = Color(255, 0, 0);
            data[i][1] = Color(255, 0, 0);
            data[i][2] = Color(255, 255, 0);
            data[i][3] = Color(255, 255, 0);
            data[i][4] = Color(0, 255, 0);
            data[i][5] = Color(0, 255, 0);
            data[i][6] = Color(0, 255, 255);
            data[i][7] = Color(0, 255, 255);
            data[i][8] = Color(0, 0, 255);
            data[i][9] = Color(0, 0, 255);
            data[i][10] = Color(255, 0, 255);
            data[i][11] = Color(255, 0, 255);
        } else {
            for (int j = 1; j <= 12; ++j) {
                uchar tone = (j*255/12);
                data[i][j-1] = Color(tone, tone, tone);
            }
        }
    }
    
    types::Image image = types::Image(8, 12, data);
    
    types::PNG png = types::PNG(image);
    
    png.save("./test_png.png");
    
    // TODO: Add expected file output and assert equivalent
}

void run_png_tests() {
    TEST(test_png_save)
}
