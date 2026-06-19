#include <gtest/gtest.h>
#include "image.hpp"
#include "gaussian.hpp"
#include "sobel.hpp"
#include "nms.hpp"
#include "hysteresis.hpp"
#include <cstdio>

TEST(Phase3_Gaussian, UniformImage) {
    Image uniform(128, 128);
    for (int i = 0; i < 128*128; i++) uniform.setPixel(i % 128, i / 128, 128);
    Image blurred = gaussianBlur(uniform);
    EXPECT_NEAR(blurred.getPixel(64, 64), 128, 2);
}

TEST(Phase3_Gaussian, Impulse) {
    Image impulse(128, 128);
    impulse.setPixel(64, 64, 255);
    Image blurred = gaussianBlur(impulse);
    EXPECT_GT(blurred.getPixel(64, 64), 100);
    EXPECT_GT(blurred.getPixel(63, 64), 10);
}

TEST(Phase3_Sobel, VerticalEdge) {
    Image vert(96, 96);
    for (int y = 0; y < 96; y++)
        for (int x = 0; x < 96; x++)
            vert.setPixel(x, y, x < 48 ? 0 : 255);
    Image blur = gaussianBlur(vert);
    SobelResult result = sobelGradient(blur);
    EXPECT_GT(result.magnitude.getPixel(48, 48), 50);
    EXPECT_EQ(result.direction.getPixel(48, 48), 0);
}

TEST(Phase3_Sobel, HorizontalEdge) {
    Image horiz(96, 96);
    for (int y = 0; y < 96; y++)
        for (int x = 0; x < 96; x++)
            horiz.setPixel(x, y, y < 48 ? 0 : 255);
    Image blur = gaussianBlur(horiz);
    SobelResult result = sobelGradient(blur);
    EXPECT_GT(result.magnitude.getPixel(48, 48), 50);
    EXPECT_EQ(result.direction.getPixel(48, 48), 2);
}

TEST(Phase3_Pipeline, FullCanny) {
    Image input(64, 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            input.setPixel(x, y, x < 32 ? 0 : 255);
    Image blur = gaussianBlur(input);
    SobelResult sobel = sobelGradient(blur);
    Image nms = nonMaxSuppression(sobel.magnitude, sobel.direction);
    Image edges = hysteresisThreshold(nms, 20, 60);
    EXPECT_EQ(edges.getWidth(), 64);
    EXPECT_EQ(edges.getHeight(), 64);
    EXPECT_GT(edges.getPixel(32, 32), 100);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
