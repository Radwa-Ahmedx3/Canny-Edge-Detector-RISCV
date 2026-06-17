#include "image.hpp"
#include "gaussian.hpp"
#include "gaussian_rvv.hpp"
#include "sobel.hpp"
#include "sobel_rvv.hpp"
#include <iostream>
#include <cstdlib>

// Check if two images are equivalent (allow +-2 tolerance for rounding)
bool imagesEqual(const Image& a, const Image& b, int tolerance = 2) {
    if (a.getWidth() != b.getWidth() || a.getHeight() != b.getHeight())
        return false;
    int w = a.getWidth(), h = a.getHeight();
    int maxDiff = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int diff = abs((int)a.getPixel(x,y) - (int)b.getPixel(x,y));
            if (diff > maxDiff) maxDiff = diff;
            if (diff > tolerance) {
                std::cout << "MISMATCH at (" << x << "," << y << "): "
                          << (int)a.getPixel(x,y) << " vs "
                          << (int)b.getPixel(x,y) << std::endl;
                return false;
            }
        }
    }
    std::cout << "Max difference: " << maxDiff << std::endl;
    return true;
}

int main() {
    int w = 100, h = 75; // Non-power-of-two to test strip-mining tail case

    // Create test image: white square on black background
    Image input(w, h);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            bool inSquare = (x>=20 && x<70 && y>=15 && y<55);
            input.setPixel(x, y, inSquare ? 255 : 0);
        }

    // ===== Test Gaussian =====
    std::cout << "=== Gaussian Equivalence Test ===" << std::endl;
    Image scalar_blur = gaussianBlur(input);
    Image rvv_blur    = gaussianBlurRVV(input);
    if (imagesEqual(scalar_blur, rvv_blur, 15))
        std::cout << "[PASS] Gaussian RVV matches scalar" << std::endl;
    else
        std::cout << "[FAIL] Gaussian RVV does NOT match scalar" << std::endl;

    // ===== Test Sobel =====
    std::cout << "\n=== Sobel Equivalence Test ===" << std::endl;
    SobelResult scalar_sobel = sobelGradient(scalar_blur);
    SobelResult rvv_sobel    = sobelGradientRVV(scalar_blur);

    std::cout << "Magnitude: ";
    if (imagesEqual(scalar_sobel.magnitude, rvv_sobel.magnitude))
        std::cout << "[PASS] Sobel magnitude RVV matches scalar" << std::endl;
    else
        std::cout << "[FAIL] Sobel magnitude RVV does NOT match scalar" << std::endl;

    return 0;
}
