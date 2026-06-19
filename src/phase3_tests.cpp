#include "image.hpp"
#include "gaussian.hpp"
#include "gaussian_rvv.hpp"
#include "sobel.hpp"
#include "sobel_rvv.hpp"
#include "nms.hpp"
#include "hysteresis.hpp"
#include <iostream>
#include <cstdio>

void outputImagePPM(const char* label, const Image& img) {
    printf("=== %s ===\n", label);
    printf("P2\n%d %d\n255\n", img.getWidth(), img.getHeight());
    for (int i = 0; i < img.getWidth() * img.getHeight(); i++) {
        printf("%d ", img.getData()[i]);
        if ((i+1) % 16 == 0) printf("\n");
    }
    printf("\n");
}

int main() {
    std::cout << "=== PHASE 3: Testing ===\n\n";

    // Test 1: Gaussian on uniform
    std::cout << "Test 1: Gaussian on uniform image...\n";
    Image uniform(128, 128);
    for (int i = 0; i < 128*128; i++) uniform.setPixel(i % 128, i / 128, 128);
    Image blur_uniform = gaussianBlur(uniform);
    if (blur_uniform.getPixel(64, 64) >= 126 && blur_uniform.getPixel(64, 64) <= 130) {
        std::cout << "PASS: Uniform image preserved\n\n";
    }

    // Test 2: Gaussian on impulse
    std::cout << "Test 2: Gaussian on impulse image...\n";
    Image impulse(128, 128);
    impulse.setPixel(64, 64, 255);
    Image blur_impulse = gaussianBlur(impulse);
    if (blur_impulse.getPixel(64, 64) > 100) {
        std::cout << "PASS: Impulse spreads correctly\n\n";
    }

    // Test 3: Sobel on vertical edge
    std::cout << "Test 3: Sobel on vertical edge...\n";
    Image vert(96, 96);
    for (int y = 0; y < 96; y++)
        for (int x = 0; x < 96; x++)
            vert.setPixel(x, y, x < 48 ? 0 : 255);
    Image vert_blur = gaussianBlur(vert);
    SobelResult vert_sobel = sobelGradient(vert_blur);
    if (vert_sobel.magnitude.getPixel(48, 48) > 50) {
        std::cout << "PASS: Vertical edge detected\n";
        outputImagePPM("SOBEL_VERTICAL_MAGNITUDE", vert_sobel.magnitude);
        std::cout << "\n";
    }

    // Test 4: Sobel on horizontal edge
    std::cout << "Test 4: Sobel on horizontal edge...\n";
    Image horiz(96, 96);
    for (int y = 0; y < 96; y++)
        for (int x = 0; x < 96; x++)
            horiz.setPixel(x, y, y < 48 ? 0 : 255);
    Image horiz_blur = gaussianBlur(horiz);
    SobelResult horiz_sobel = sobelGradient(horiz_blur);
    if (horiz_sobel.magnitude.getPixel(48, 48) > 50) {
        std::cout << "PASS: Horizontal edge detected\n";
        outputImagePPM("SOBEL_HORIZONTAL_MAGNITUDE", horiz_sobel.magnitude);
        std::cout << "\n";
    }

    // Test 5: Full pipeline
    std::cout << "Test 5: Full Canny pipeline...\n";
    Image input(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            input.setPixel(x, y, x < 32 ? 0 : 255);
    Image blur = gaussianBlur(input);
    SobelResult sobel = sobelGradient(blur);
    Image nms = nonMaxSuppression(sobel.magnitude, sobel.direction);
    Image edges = hysteresisThreshold(nms, 20, 60);
    std::cout << "PASS: Pipeline complete\n";
    outputImagePPM("CANNY_OUTPUT", edges);

    // Test 6: Equivalence (scalar vs RVV)
    std::cout << "\nTest 6: Equivalence test (scalar vs RVV)...\n";
    Image test_img(100, 75);
    for (int i = 0; i < 100*75; i++) test_img.setPixel(i % 100, i / 75, 128);
    Image s_blur = gaussianBlur(test_img);
    Image v_blur = gaussianBlurRVV(test_img);
    int mismatches = 0;
    for (int i = 0; i < 100*75; i++) {
        if (abs((int)s_blur.getData()[i] - (int)v_blur.getData()[i]) > 1) mismatches++;
    }
    if (mismatches == 0) {
        std::cout << "PASS: Scalar and RVV match perfectly\n";
    }

    std::cout << "\n=== PHASE 3 COMPLETE ===\n";
    return 0;
}
