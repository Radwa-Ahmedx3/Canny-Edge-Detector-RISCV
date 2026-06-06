#ifndef HYSTERESIS_HPP
#define HYSTERESIS_HPP

#include "image.hpp"

// Hysteresis Thresholding
// بتاخد صورة وحدين: strong threshold و weak threshold
// وبترجع صورة فيها الـ edges النهائية
Image hysteresisThreshold(const Image& input, 
                          uint8_t lowThresh, 
                          uint8_t highThresh);

#endif