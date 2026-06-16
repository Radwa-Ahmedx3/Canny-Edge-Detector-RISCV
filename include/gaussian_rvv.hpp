#ifndef GAUSSIAN_RVV_HPP
#define GAUSSIAN_RVV_HPP

#include "image.hpp"

// RVV-optimized Gaussian Blur
// Uses LMUL=2 for better throughput on 8-bit pixels
Image gaussianBlurRVV(const Image& input);

#endif
