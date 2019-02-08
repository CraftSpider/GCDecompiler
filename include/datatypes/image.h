#pragma once

#include "types.h"
#include "datatypes/color.h"

namespace types {

class Image {

public:
    
    uint height, width;
    Color **image_data;
    
    Image();
    
    Image(uint height, uint width, Color **image_data);
    
    Image(const Image &image);
    
    Image(Image &&image) noexcept;
    
    ~Image();
    
    void operator=(const Image &image);
    
};

}
