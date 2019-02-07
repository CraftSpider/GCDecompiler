//
// Created by Rune Tynan on 8/11/2018.
//

#include "filetypes/image.h"

namespace types {

uint Color::to_int() {
    uint out = 0;
    out += R << 24U;
    out += G << 16U;
    out += B << 8U;
    out += A;
    return out;
}

Color Color::lerp_colors(const types::Color &a, const types::Color &b, const float &factor) {
    return Color {
            (uchar) (a.R + (b.R - a.R) * factor),
            (uchar) (a.G + (b.G - a.G) * factor),
            (uchar) (a.B + (b.B - a.B) * factor),
            (uchar) (a.A + (b.A - a.A) * factor)
    };
}

Image::Image() {
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
