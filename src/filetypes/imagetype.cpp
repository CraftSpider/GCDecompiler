//
// Created by Rune Tynan on 8/11/2018.
//

#include "filetypes/imagetype.h"

namespace types {

SingleImageType::SingleImageType() = default;

SingleImageType::SingleImageType(const Image& image) {
    _image = image;
}

SingleImageType::~SingleImageType() = default;

Image SingleImageType::get_image() {
    return _image;
}

const std::string SingleImageType::image_type = "SingleImageType";

MultiImageType::MultiImageType() = default;

MultiImageType::MultiImageType(Image *images, uint num_imgs) {
    _num_images = num_imgs;
    _images = images;
}

uint MultiImageType::num_images() {
    return _num_images;
}

Image* MultiImageType::get_images() {
    return _images;
}

const std::string MultiImageType::image_type = "MultiImageType";

}
