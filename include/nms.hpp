#ifndef NMS_HPP
#define NMS_HPP

#include "image.hpp"

// Non-Maximum Suppression
// بتاخد الـ magnitude والـ direction وبترجع صورة فيها الـ edges رفيعة
Image nonMaxSuppression(const Image& magnitude, const Image& direction);

#endif