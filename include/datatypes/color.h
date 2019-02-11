#pragma once

#include "types.h"

namespace types {

struct Color {
    uchar R, G, B, A;
    
    Color();
    Color(uchar R, uchar G, uchar B);
    Color(uchar R, uchar G, uchar B, uchar A);
    
    bool operator==(const Color& other);
    
    uint to_int();
    
    static Color lerp_colors(const Color &a, const Color &b, const float &factor);
};

}
