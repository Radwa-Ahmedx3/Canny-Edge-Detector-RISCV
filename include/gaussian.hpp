#ifndef GAUSSIAN_HPP
#define GAUSSIAN_HPP

#include "image.hpp"

Image gaussianBlur(const Image& input);

#endif
Image gaussianBlurPadded(const Image& input);