#ifndef SOBEL_RVV_HPP
#define SOBEL_RVV_HPP

#include "image.hpp"
#include "sobel.hpp"

// RVV-optimized Sobel Gradient
// Computes L1 magnitude and direction using vector operations
SobelResult sobelGradientRVV(const Image& input);

#endif
