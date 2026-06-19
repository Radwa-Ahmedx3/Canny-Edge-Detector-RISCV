// phase3_test.cpp - Nada Tamer - Phase 3 Tests
// Tests: Gaussian + Sobel + Direction + Magnitude (no gtest needed)
// Build: g++ -I include phase3_test.cpp src/image.cpp src/gaussian.cpp src/sobel.cpp -o phase3_test
// Run:   ./phase3_test

#include "../include/image.hpp"
#include "../include/gaussian.hpp"
#include "../include/sobel.hpp"
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>

// ============================================================
// Helper: طباعة grid صغيرة من البيكسلات للـ visual output
// ============================================================
void printGrid(const Image& img, int startX, int startY, int size, const std::string& label) {
    std::cout << "\n[Visual Output] " << label << " (" << size << "x" << size << " region at " 
              << startX << "," << startY << "):\n";
    for (int y = startY; y < startY + size && y < img.getHeight(); y++) {
        for (int x = startX; x < startX + size && x < img.getWidth(); x++) {
            int val = img.getPixel(x, y);
            // طباعة رمز بصري بدل الأرقام
            if      (val == 0)         std::cout << " .  ";
            else if (val < 64)         std::cout << " +  ";
            else if (val < 128)        std::cout << " #  ";
            else if (val < 200)        std::cout << " X  ";
            else                       std::cout << "255 ";
        }
        std::cout << "\n";
    }
}

void printDirectionGrid(const Image& img, int startX, int startY, int size, const std::string& label) {
    std::cout << "\n[Direction Output] " << label << " (0=horiz, 1=diag/, 2=vert, 3=diag\\):\n";
    for (int y = startY; y < startY + size && y < img.getHeight(); y++) {
        for (int x = startX; x < startX + size && x < img.getWidth(); x++) {
            int dir = img.getPixel(x, y);
            if      (dir == 0) std::cout << " —  ";
            else if (dir == 1) std::cout << " /  ";
            else if (dir == 2) std::cout << " |  ";
            else               std::cout << " \\  ";
        }
        std::cout << "\n";
    }
}

// ============================================================
// TEST 1: Black Image → Gaussian → يفضل black
// ============================================================
void test_black_image_gaussian() {
    std::cout << "\n========================================\n";
    std::cout << "TEST 1: Black Image → Gaussian Blur\n";
    std::cout << "Expected: all pixels stay 0\n";
    std::cout << "========================================\n";

    Image img(32, 32);
    for (int y = 0; y < 32; y++)
        for (int x = 0; x < 32; x++)
            img.setPixel(x, y, 0);

    Image blurred = gaussianBlur(img);

    // Visual output
    printGrid(blurred, 0, 0, 8, "Black Image after Gaussian");

    // Check
    int failures = 0;
    for (int y = 0; y < 32; y++)
        for (int x = 0; x < 32; x++)
            if (blurred.getPixel(x, y) != 0) failures++;

    if (failures == 0)
        std::cout << "PASS: All pixels = 0 ✓\n";
    else
        std::cout << "FAIL: " << failures << " pixels != 0 ✗\n";

    assert(failures == 0);
}

// ============================================================
// TEST 2: Black Image + Single Bright Pixel → Gaussian → ينتشر
// ============================================================
void test_impulse_gaussian() {
    std::cout << "\n========================================\n";
    std::cout << "TEST 2: Single Bright Pixel (impulse) → Gaussian Blur\n";
    std::cout << "Expected: center stays bright, neighbors nonzero (spread)\n";
    std::cout << "========================================\n";

    Image img(32, 32);
    for (int y = 0; y < 32; y++)
        for (int x = 0; x < 32; x++)
            img.setPixel(x, y, 0);
    img.setPixel(16, 16, 255);  // نقطة واحدة بيضاء في المنتصف

    Image blurred = gaussianBlur(img);

    // Visual output - المنطقة حوالين المنتصف
    printGrid(blurred, 11, 11, 10, "Impulse after Gaussian (center region)");

    // Center should be nonzero
    int center = blurred.getPixel(16, 16);
    std::cout << "Center pixel value: " << center << " (expected > 0)\n";
    assert(center > 0);

    // Neighbors should be nonzero (الـ blur ينتشر للجيران)
    assert(blurred.getPixel(17, 16) > 0);
    assert(blurred.getPixel(16, 17) > 0);
    assert(blurred.getPixel(15, 16) > 0);
    assert(blurred.getPixel(16, 15) > 0);

    // Corner far away should be zero
    assert(blurred.getPixel(0, 0) == 0);

    // Symmetry check: اليمين = الشمال (kernel متماثل)
    int right = blurred.getPixel(17, 16);
    int left  = blurred.getPixel(15, 16);
    int down  = blurred.getPixel(16, 17);
    int up    = blurred.getPixel(16, 15);
    std::cout << "Symmetry check - Left:" << left << " Right:" << right 
              << " Up:" << up << " Down:" << down << "\n";
    assert(abs(right - left) <= 1);
    assert(abs(up - down) <= 1);

    std::cout << "PASS: Impulse spreads symmetrically ✓\n";
}

// ============================================================
// TEST 3: Vertical Edge → Sobel + Direction
// ============================================================
void test_vertical_edge_sobel_direction() {
    std::cout << "\n========================================\n";
    std::cout << "TEST 3: Vertical Edge → Sobel + Direction\n";
    std::cout << "Expected: high magnitude at edge, direction = 0 (horizontal gradient)\n";
    std::cout << "========================================\n";

    // صورة: نص يسار = أسود (0)، نص يمين = أبيض (255)
    Image img(32, 32);
    for (int y = 0; y < 32; y++)
        for (int x = 0; x < 32; x++)
            img.setPixel(x, y, x < 16 ? 0 : 255);

    SobelResult result = sobelGradient(img);

    // Visual output للـ magnitude
    printGrid(result.magnitude, 12, 12, 8, "Vertical Edge - Magnitude");
    
    // Visual output للـ direction
    printDirectionGrid(result.direction, 12, 12, 8, "Vertical Edge - Direction");

    // Edge at x=16 should have high magnitude
    int mag_at_edge = result.magnitude.getPixel(16, 16);
    std::cout << "Magnitude at edge (16,16): " << mag_at_edge << " (expected > 0)\n";
    assert(mag_at_edge > 0);

    // Interior يسار الـ edge = zero magnitude
    assert(result.magnitude.getPixel(5, 16) == 0);
    // Interior يمين الـ edge = zero magnitude
    assert(result.magnitude.getPixel(27, 16) == 0);

    // Direction at edge should be 0 (horizontal gradient = vertical edge)
    int dir_at_edge = result.direction.getPixel(16, 16);
    std::cout << "Direction at edge (16,16): " << dir_at_edge << " (expected 0 = horizontal gradient)\n";
    assert(dir_at_edge == 0);

    std::cout << "PASS: Vertical edge detected with correct direction ✓\n";
}

// ============================================================
// TEST 4: Horizontal Edge → Sobel + Direction
// ============================================================
void test_horizontal_edge_sobel_direction() {
    std::cout << "\n========================================\n";
    std::cout << "TEST 4: Horizontal Edge → Sobel + Direction\n";
    std::cout << "Expected: high magnitude at edge, direction = 2 (vertical gradient)\n";
    std::cout << "========================================\n";

    // صورة: نص فوق = أسود (0)، نص تحت = أبيض (255)
    Image img(32, 32);
    for (int y = 0; y < 32; y++)
        for (int x = 0; x < 32; x++)
            img.setPixel(x, y, y < 16 ? 0 : 255);

    SobelResult result = sobelGradient(img);

    // Visual output
    printGrid(result.magnitude, 12, 12, 8, "Horizontal Edge - Magnitude");
    printDirectionGrid(result.direction, 12, 12, 8, "Horizontal Edge - Direction");

    // Edge at y=16 should have high magnitude
    int mag_at_edge = result.magnitude.getPixel(16, 16);
    std::cout << "Magnitude at edge (16,16): " << mag_at_edge << " (expected > 0)\n";
    assert(mag_at_edge > 0);

    // Direction should be 2 (vertical gradient = horizontal edge)
    int dir_at_edge = result.direction.getPixel(16, 16);
    std::cout << "Direction at edge (16,16): " << dir_at_edge << " (expected 2 = vertical gradient)\n";
    assert(dir_at_edge == 2);

    std::cout << "PASS: Horizontal edge detected with correct direction ✓\n";
}

// ============================================================
// TEST 5: Diagonal Edge → Sobel + Direction
// ============================================================
void test_diagonal_edge_sobel_direction() {
    std::cout << "\n========================================\n";
    std::cout << "TEST 5: Diagonal Edge → Sobel + Direction\n";
    std::cout << "Expected: significant Gx AND Gy, direction = 1 or 3\n";
    std::cout << "========================================\n";

    // صورة: فوق اليسار = أبيض، تحت اليمين = أسود (edge على القطر)
    Image img(32, 32);
    for (int y = 0; y < 32; y++)
        for (int x = 0; x < 32; x++)
            img.setPixel(x, y, (x + y < 32) ? 255 : 0);

    SobelResult result = sobelGradient(img);

    // Visual output
    printGrid(result.magnitude, 10, 10, 10, "Diagonal Edge - Magnitude");
    printDirectionGrid(result.direction, 10, 10, 10, "Diagonal Edge - Direction");

    // Edge pixels on the diagonal should have nonzero magnitude
    int mag1 = result.magnitude.getPixel(15, 15);
    int mag2 = result.magnitude.getPixel(16, 16);
    std::cout << "Magnitude at diagonal (15,15): " << mag1 << "\n";
    std::cout << "Magnitude at diagonal (16,16): " << mag2 << "\n";
    assert(mag1 > 0 || mag2 > 0);

    // Direction should be 1 or 3 (diagonal)
    int dir1 = result.direction.getPixel(15, 15);
    int dir2 = result.direction.getPixel(16, 16);
    std::cout << "Direction at (15,15): " << dir1 << " (expected 1 or 3)\n";
    std::cout << "Direction at (16,16): " << dir2 << " (expected 1 or 3)\n";
    assert(dir1 == 1 || dir1 == 3 || dir2 == 1 || dir2 == 3);

    std::cout << "PASS: Diagonal edge detected with correct direction ✓\n";
}

// ============================================================
// TEST 6: Magnitude nonzero on random-ish image (L1 and L2)
// ============================================================
void test_magnitude_nonzero() {
    std::cout << "\n========================================\n";
    std::cout << "TEST 6: Magnitude nonzero (L1 and L2) on non-uniform image\n";
    std::cout << "Expected: both L1 and L2 produce nonzero output\n";
    std::cout << "========================================\n";

    // صورة فيها variation: checkerboard بسيط
    Image img(32, 32);
    for (int y = 0; y < 32; y++)
        for (int x = 0; x < 32; x++)
            img.setPixel(x, y, x < 16 ? 0 : 255);

    SobelResult resultL1 = sobelGradient(img);    // L1
    SobelResult resultL2 = sobelGradientL2(img);  // L2

    // Count nonzero pixels
    int nonzero_L1 = 0, nonzero_L2 = 0;
    for (int y = 1; y < 31; y++)
        for (int x = 1; x < 31; x++) {
            if (resultL1.magnitude.getPixel(x, y) > 0) nonzero_L1++;
            if (resultL2.magnitude.getPixel(x, y) > 0) nonzero_L2++;
        }

    std::cout << "L1 nonzero pixels: " << nonzero_L1 << " (expected > 0)\n";
    std::cout << "L2 nonzero pixels: " << nonzero_L2 << " (expected > 0)\n";

    // Visual comparison
    printGrid(resultL1.magnitude, 12, 12, 8, "L1 Magnitude");
    printGrid(resultL2.magnitude, 12, 12, 8, "L2 Magnitude");

    assert(nonzero_L1 > 0);
    assert(nonzero_L2 > 0);

    std::cout << "PASS: Both L1 and L2 produce nonzero magnitude ✓\n";
}

// ============================================================
// MAIN
// ============================================================
int main() {
    std::cout << "==============================================\n";
    std::cout << "  Phase 3 Tests - Nada Tamer\n";
    std::cout << "  Gaussian + Sobel + Direction + Magnitude\n";
    std::cout << "==============================================\n";

    test_black_image_gaussian();
    test_impulse_gaussian();
    test_vertical_edge_sobel_direction();
    test_horizontal_edge_sobel_direction();
    test_diagonal_edge_sobel_direction();
    test_magnitude_nonzero();

    std::cout << "\n==============================================\n";
    std::cout << "  ALL 6 TESTS PASSED ✓\n";
    std::cout << "==============================================\n";

    return 0;
}
