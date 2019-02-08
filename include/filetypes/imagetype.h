//
// Created by Rune Tynan on 8/11/2018.
//

#pragma once

#include <string>
#include <stdexcept>

#include "types.h"
#include "datatypes/image.h"

namespace types {

class ImageType {

protected:
    
    static std::string image_type;
    
};

class SingleImageType : public ImageType {

protected:
    
    Image _image;
    
    SingleImageType();

public:
    
    static const std::string image_type;
    
    SingleImageType(const Image& image);
    virtual Image get_image();
    
};

class MultiImageType : public ImageType {

protected:
    
    uint _num_images;
    Image* _images;
    
    MultiImageType();

public:
    
    static const std::string image_type;
    
    MultiImageType(Image* images, uint num_imgs);
    virtual uint num_images();
    virtual Image* get_images();
    
};

template<class To, class From>
To* convert_image(From* img);

template<class To, class From>
To* from_single_image(From* img);

template<class To, class From>
To* from_multi_image(From* img);

#include "imagetype.tpp"

}
