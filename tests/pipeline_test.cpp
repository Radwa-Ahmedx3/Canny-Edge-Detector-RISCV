#include <gtest/gtest.h>
#include "../include/image.hpp"
#include "../include/gaussian.hpp"
#include "../include/sobel.hpp"
#include "../include/nms.hpp"
#include "../include/hysteresis.hpp"

// ===== GAUSSIAN TESTS =====

TEST(GaussianTest, UniformImageStaysUniform) {
    Image img(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            img.setPixel(x, y, 128);

    Image blurred = gaussianBlur(img);

    // Interior pixels should stay ~128 (allow ±2 for rounding)
    for (int y = 3; y < 61; y++)
        for (int x = 3; x < 61; x++)
            EXPECT_NEAR(blurred.getPixel(x, y), 128, 2);
}

TEST(GaussianTest, BlackImageStaysBlack) {
    Image img(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            img.setPixel(x, y, 0);

    Image blurred = gaussianBlur(img);

    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            EXPECT_EQ(blurred.getPixel(x, y), 0);
}

TEST(GaussianTest, ImpulseSpreadToNeighbors) {
    Image img(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            img.setPixel(x, y, 0);
    img.setPixel(32, 32, 255);

    Image blurred = gaussianBlur(img);

    // Center should be nonzero
    EXPECT_GT(blurred.getPixel(32, 32), 0);
    // Neighbors should be nonzero
    EXPECT_GT(blurred.getPixel(33, 32), 0);
    EXPECT_GT(blurred.getPixel(32, 33), 0);
}

// ===== SOBEL TESTS =====

TEST(SobelTest, UniformImageZeroGradient) {
    Image img(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            img.setPixel(x, y, 128);

    SobelResult result = sobelGradient(img);

    // Interior pixels should have zero magnitude
    for (int y = 2; y < 62; y++)
        for (int x = 2; x < 62; x++)
            EXPECT_EQ(result.magnitude.getPixel(x, y), 0);
}

TEST(SobelTest, VerticalEdgeDetected) {
    Image img(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            img.setPixel(x, y, x < 32 ? 0 : 255);

    SobelResult result = sobelGradient(img);

    // Edge at x=32 should have high magnitude
    EXPECT_GT(result.magnitude.getPixel(32, 32), 0);
}

TEST(SobelTest, HorizontalEdgeDetected) {
    Image img(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            img.setPixel(x, y, y < 32 ? 0 : 255);

    SobelResult result = sobelGradient(img);

    // Edge at y=32 should have high magnitude
    EXPECT_GT(result.magnitude.getPixel(32, 32), 0);
}

// ===== NMS TESTS =====

TEST(NMSTest, OutputSmallerThanInput) {
    Image img(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            img.setPixel(x, y, x < 32 ? 0 : 255);

    SobelResult result = sobelGradient(img);
    Image thinned = nonMaxSuppression(result.magnitude, result.direction);

    int mag_nonzero = 0, nms_nonzero = 0;
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++) {
            if (result.magnitude.getPixel(x, y) > 0) mag_nonzero++;
            if (thinned.getPixel(x, y) > 0) nms_nonzero++;
        }

    // NMS should reduce number of edge pixels
    EXPECT_LE(nms_nonzero, mag_nonzero);
}

// ===== HYSTERESIS TESTS =====

TEST(HysteresisTest, StrongEdgesKept) {
    Image img(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            img.setPixel(x, y, x < 32 ? 0 : 255);

    SobelResult result = sobelGradient(img);
    Image thinned = nonMaxSuppression(result.magnitude, result.direction);
    Image edges = hysteresisThreshold(thinned, 20, 80);

    // There should be some detected edges
    int edge_count = 0;
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            if (edges.getPixel(x, y) == 255) edge_count++;

    EXPECT_GT(edge_count, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
