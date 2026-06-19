#ifndef GAUSSIAN_HPP
#define GAUSSIAN_HPP
#include "image.hpp"
Image gaussianBlur(const Image& input);
Image gaussianBlurPadded(const Image& input);
Image gaussianBlurRaw(const Image& input);
Image gaussianBlurRVV(const Image& input);
Image gaussianBlurRVV_m1(const Image& input);
Image gaussianBlurRVV_m4(const Image& input);
Image gaussianBlurRVV_fp(const Image& input);
#endif
