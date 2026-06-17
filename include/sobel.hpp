#ifndef SOBEL_HPP
#define SOBEL_HPP

#include "image.hpp"

// بتحسب الـ gradient في الاتجاهين X وY
// وبترجع الـ magnitude والـ direction
struct SobelResult {
    Image magnitude;  // قوة الـ edge
    Image direction;  // اتجاه الـ edge: 0, 1, 2, 3
};

SobelResult sobelGradient(const Image& input);    // L1 norm: |Gx| + |Gy|
SobelResult sobelGradientL2(const Image& input);  // L2 norm: sqrt(Gx^2 + Gy^2)

#endif