#include "image.hpp"
#include "gaussian.hpp"
#include "gaussian_rvv.hpp"
#include "sobel.hpp"
#include "sobel_rvv.hpp"
#include <iostream>
#include <cstdlib>

int main() {
    std::cout << "=== EQUIVALENCE TESTING: ±1 TOLERANCE VERIFICATION ===\n\n";

    // Test 1: Impulse (will have rounding)
    std::cout << "[Test 1] Impulse Image (100x75)\n";
    Image impulse(100, 75);
    impulse.setPixel(50, 37, 255);
    
    Image s_gauss = gaussianBlur(impulse);
    Image v_gauss = gaussianBlurRVV(impulse);
    
    int diffs_found = 0;
    int max_diff = 0;
    for (int i = 0; i < 100*75; i++) {
        int diff = (int)s_gauss.getData()[i] - (int)v_gauss.getData()[i];
        if (diff < 0) diff = -diff;
        if (diff > 0) diffs_found++;
        if (diff > max_diff) max_diff = diff;
    }
    
    printf("  Differences found: %d pixels\n", diffs_found);
    printf("  Max difference: %d\n", max_diff);
    printf("  Within ±1: %s\n", max_diff <= 1 ? "YES ✓" : "NO ✗");
    
    // Test 2: Random-ish image
    std::cout << "\n[Test 2] Mixed Values Image (100x75)\n";
    Image mixed(100, 75);
    for (int i = 0; i < 100*75; i++) {
        mixed.setPixel(i % 100, i / 100, (i * 7) % 256);
    }
    
    s_gauss = gaussianBlur(mixed);
    v_gauss = gaussianBlurRVV(mixed);
    
    diffs_found = 0;
    max_diff = 0;
    int exceed_tolerance = 0;
    for (int i = 0; i < 100*75; i++) {
        int diff = (int)s_gauss.getData()[i] - (int)v_gauss.getData()[i];
        if (diff < 0) diff = -diff;
        if (diff > 0) diffs_found++;
        if (diff > max_diff) max_diff = diff;
        if (diff > 1) exceed_tolerance++;
    }
    
    printf("  Differences found: %d pixels\n", diffs_found);
    printf("  Max difference: %d\n", max_diff);
    printf("  Pixels exceeding ±1: %d\n", exceed_tolerance);
    printf("  Within ±1: %s\n", exceed_tolerance == 0 ? "YES ✓" : "NO ✗");
    
    if (exceed_tolerance == 0) {
        std::cout << "\n=== TOLERANCE VERIFIED: All differences within ±1 ===\n";
        return 0;
    }
    return 1;
}
