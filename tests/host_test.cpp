#include <gtest/gtest.h>
#include <vector>

int vector_sum(const std::vector<int>& v) {
    int sum = 0;
    for (int x : v)
        sum += x;
    return sum;
}

TEST(VectorTest, SumPositiveNumbers) {
    std::vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(vector_sum(v), 10);
}

TEST(VectorTest, SumWithZero) {
    std::vector<int> v = {0, 5, 0};
    EXPECT_EQ(vector_sum(v), 5);
}

TEST(VectorTest, EmptyVector) {
    std::vector<int> v;
    EXPECT_EQ(vector_sum(v), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
