#include "image.hpp"
#include "gaussian.hpp"
#include "gaussian_rvv.hpp"
#include "sobel.hpp"
#include "sobel_rvv.hpp"
#include "test_image_generator.hpp"
#include <iostream>
#include <cstdlib>

int main() {
    std::cout << "=== EQUIVALENCE TESTING: Scalar vs RVV (±1 tolerance) ===\n\n";

    // Test with 100x75 (non-power-of-two for VLA tail case)
    Image test = TestImageGenerator::uniformImage(100, 75, 128);
    
    std::cout << "Test image: 100x75 (non-power-of-two)\n";
    std::cout << "Tolerance: ±1\n\n";

    // Gaussian comparison
    std::cout << "Gaussian Blur:\n";
    Image scalar = gaussianBlur(test);
    Image rvv = gaussianBlurRVV(test);
    
    int mismatches = 0;
    int max_diff = 0;
    for (int i = 0; i < 100*75; i++) {
        int diff = (int)scalar.getData()[i] - (int)rvv.getData()[i];
        if (diff < 0) diff = -diff;
        if (diff > max_diff) max_diff = diff;
        if (diff > 1) mismatches++;
    }
    
    printf("  Max difference: %d\n", max_diff);
    printf("  Pixels exceeding ±1: %d\n", mismatches);
    if (mismatches == 0 && max_diff <= 1) {
        std::cout << "  ✓ PASS (all differences within ±1)\n\n";
    } else {
        std::cout << "  ✗ FAIL\n\n";
        return 1;
    }

    // Sobel comparison
    std::cout << "Sobel Gradient:\n";
    SobelResult s_scalar = sobelGradient(scalar);
    SobelResult s_rvv = sobelGradientRVV(rvv);
    
    mismatches = 0;
    max_diff = 0;
    for (int i = 0; i < 100*75; i++) {
        int diff = (int)s_scalar.magnitude.getData()[i] - (int)s_rvv.magnitude.getData()[i];
        if (diff < 0) diff = -diff;
        if (diff > max_diff) max_diff = diff;
        if (diff > 1) mismatches++;
    }
    
    printf("  Max difference: %d\n", max_diff);
    printf("  Pixels exceeding ±1: %d\n", mismatches);
    if (mismatches == 0 && max_diff <= 1) {
        std::cout << "  ✓ PASS (all differences within ±1)\n\n";
    } else {
        std::cout << "  ✗ FAIL\n\n";
        return 1;
    }

    std::cout << "=== ALL EQUIVALENCE TESTS PASSED (±1 tolerance verified) ===\n";
    return 0;
}
