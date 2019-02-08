
#include "datatypes/image.h"

namespace types {

Image::Image() {
    this->height = 0;
    this->width = 0;
    this->image_data = nullptr;
}

Image::Image(uint height, uint width, types::Color **image_data) {
    this->height = height;
    this->width = width;
    this->image_data = image_data;
}

Image::Image(const types::Image &image) {
    this->height = image.height;
    this->width = image.width;
    this->image_data = new Color *[this->height];
    for (uint i = 0; i < this->height; i++) {
        this->image_data[i] = new Color[this->width];
    }
    for (uint i = 0; i < this->height; i++) {
        for (uint j = 0; j < this->width; j++) {
            this->image_data[i][j] = image.image_data[i][j];
        }
    }
}

Image::Image(types::Image &&image) noexcept {
    this->height = image.height;
    this->width = image.width;
    this->image_data = image.image_data;
    image.image_data = nullptr;
}

Image::~Image() {
    for (uint i = 0; i < this->height; i++)
        delete[] this->image_data[i];
    delete[] this->image_data;
}

void Image::operator=(const types::Image &image) {
    this->height = image.height;
    this->width = image.width;
    this->image_data = new Color *[this->height];
    for (uint i = 0; i < this->height; i++) {
        this->image_data[i] = new Color[this->width];
    }
    for (uint i = 0; i < this->height; i++) {
        for (uint j = 0; j < this->width; j++) {
            this->image_data[i][j] = image.image_data[i][j];
        }
    }
}

}