#include "image.hpp"
#include "gaussian.hpp"
#include "gaussian_rvv.hpp"
#include "sobel.hpp"
#include "sobel_rvv.hpp"
#include "nms.hpp"
#include "hysteresis.hpp"
#include "test_image_generator.hpp"
#include <iostream>

bool imagesEqual(const Image& a, const Image& b, int tol = 1) {
    if (a.getWidth() != b.getWidth() || a.getHeight() != b.getHeight()) return false;
    int w = a.getWidth(), h = a.getHeight();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int diff = (int)a.getPixel(x, y) - (int)b.getPixel(x, y);
            if (diff < -tol || diff > tol) return false;
        }
    }
    return true;
}

int main() {
    std::cout << "Test 1: Gaussian uniform...\n";
    Image u1 = TestImageGenerator::uniformImage(128, 128, 128);
    if (imagesEqual(gaussianBlur(u1), gaussianBlurRVV(u1), 1)) {
        std::cout << "PASS\n";
    } else {
        std::cout << "FAIL\n"; return 1;
    }

    std::cout << "Test 2: Gaussian impulse (100x75)...\n";
    Image imp = TestImageGenerator::impulseImage(100, 75);
    if (imagesEqual(gaussianBlur(imp), gaussianBlurRVV(imp), 1)) {
        std::cout << "PASS\n";
    } else {
        std::cout << "FAIL\n"; return 1;
    }

    std::cout << "Test 3: Sobel uniform...\n";
    Image u2 = TestImageGenerator::uniformImage(128, 128, 128);
    SobelResult s1 = sobelGradient(u2);
    SobelResult s2 = sobelGradientRVV(u2);
    if (imagesEqual(s1.magnitude, s2.magnitude, 1) && imagesEqual(s1.direction, s2.direction, 0)) {
        std::cout << "PASS\n";
    } else {
        std::cout << "FAIL\n"; return 1;
    }

    std::cout << "Test 4: Vertical edge...\n";
    Image vert = TestImageGenerator::verticalEdgeImage(96, 96);
    Image b1 = gaussianBlur(vert);
    Image b2 = gaussianBlurRVV(vert);
    SobelResult sv1 = sobelGradient(b1);
    SobelResult sv2 = sobelGradientRVV(b2);
    if (imagesEqual(sv1.magnitude, sv2.magnitude, 1)) {
        std::cout << "PASS\n";
    } else {
        std::cout << "FAIL\n"; return 1;
    }

    std::cout << "Test 5: VLA tail case (100x75)...\n";
    Image test = TestImageGenerator::diagonalEdgeImage(100, 75);
    if (imagesEqual(gaussianBlur(test), gaussianBlurRVV(test), 1)) {
        std::cout << "PASS\n";
    } else {
        std::cout << "FAIL\n"; return 1;
    }

    std::cout << "All equivalence tests PASSED!\n";
    return 0;
}
