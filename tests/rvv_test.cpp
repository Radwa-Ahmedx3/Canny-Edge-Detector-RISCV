
#include <iostream>
#include <stdint.h>
#include <gtest/gtest.h>

extern "C" {
    char* getcwd(char* buf, size_t size) { if(buf) buf[0]='/'; if(buf && size>1) buf[1]='\0'; return buf; }
    int mkdir(const char* pathname, unsigned int mode) { return -1; }
    int dup(int fd) { return -1; }
    int dup2(int oldfd, int newfd) { return -1; }
}

TEST(RVV_Test, VectorAddition) {
    const int N = 4;
    uint32_t a[N] = {10, 20, 30, 40};
    uint32_t b[N] = {1, 2, 3, 4};
    uint32_t res[N] = {0};

    asm volatile (
        "vsetvli t0, %3, e32, m1, ta, ma \n\t"
        "vle32.v v1, (%0) \n\t"
        "vle32.v v2, (%1) \n\t"
        "vadd.vv v3, v1, v2 \n\t"
        "vse32.v v3, (%2) \n\t"
        :
        : "r"(a), "r"(b), "r"(res), "r"(N)
        : "t0", "v1", "v2", "v3", "memory"
    );

    for(int i = 0; i < N; i++) {
        EXPECT_EQ(res[i], a[i] + b[i]);
    }
}

int main(int argc, char **argv) {
    std::cout << "Starting RVV C++ Validation..." << std::endl;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
