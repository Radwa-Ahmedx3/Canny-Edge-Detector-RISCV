#ifdef USE_GTEST
    #include <gtest/gtest.h>
#else
    #include <iostream>
    #define TEST(suite, name) void name()
    #define EXPECT_EQ(a, b) if((a)!=(b)){ std::cerr << "FAIL: " << #a << " != " << #b << std::endl; exit(1); }
    #define EXPECT_TRUE(a) if(!(a)){ std::cerr << "FAIL: " << #a << " is false" << std::endl; exit(1); }
#endif

#include "../include/image.hpp"

TEST(ImageTest, AlignmentAndAllocation) {
    Image img(64, 64);
    uintptr_t addr = reinterpret_cast<uintptr_t>(img.getData());
    
    EXPECT_TRUE(img.getData() != nullptr);
    EXPECT_EQ(addr % 64, 0); 
}

#ifndef USE_GTEST
int main() {
    std::cout << "Running Simple QEMU Harness..." << std::endl;
    AlignmentAndAllocation();
    std::cout << "QEMU Test Passed!" << std::endl;
    return 0;
}
#endif
