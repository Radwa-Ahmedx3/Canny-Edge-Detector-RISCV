#include <gtest/gtest.h>


TEST(MathTest, Addition) {
    EXPECT_EQ(2 + 3, 5);
    EXPECT_EQ(10 + 0, 10);
}


TEST(MathTest, Subtraction) {
    EXPECT_EQ(10 - 3, 7);
    EXPECT_EQ(5 - 5, 0);
}


TEST(MathTest, Multiplication) {
    EXPECT_EQ(4 * 3, 12);
    EXPECT_EQ(0 * 100, 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
