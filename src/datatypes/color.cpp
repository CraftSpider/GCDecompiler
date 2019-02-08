
#include "datatypes/color.h"

namespace types {

Color::Color() {
    R = 0;
    G = 0;
    B = 0;
    A = 255;
}

Color::Color(uchar R, uchar G, uchar B) {
    this->R = R;
    this->G = G;
    this->B = B;
    A = 255;
}

Color::Color(uchar R, uchar G, uchar B, uchar A) {
    this->R = R;
    this->G = G;
    this->B = B;
    this->A = A;
}

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

}