#ifndef SOBEL_HPP
#define SOBEL_HPP

#include "image.hpp"

// بتحسب الـ gradient في الاتجاهين X وY
// وبترجع الـ magnitude والـ direction
struct SobelResult {
    Image magnitude;  // قوة الـ edge
    Image direction;  // اتجاه الـ edge: 0, 1, 2, 3
};

SobelResult sobelGradient(const Image& input);

SobelResult sobelGradientRVV(const Image& input);
#endif