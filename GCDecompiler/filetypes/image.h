//
// Created by Rune Tynan on 8/11/2018.
//

#pragma once

#include "types.h"

namespace types {

struct Color {
    uchar R, G, B, A;
    
    uint to_int();
    
    static Color lerp_colors(const Color &a, const Color &b, const float &factor);
};

class Image {

public:
    
    uint height, width;
    Color **image_data;
    
    Image();
    
    Image(uint height, uint width, Color **image_data);
    
    Image(const Image& image);
    
    Image(Image &&image);
    
    ~Image();
    
    void operator=(const Image& image);
    
};

}
