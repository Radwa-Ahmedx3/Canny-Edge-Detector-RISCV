
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

extern "C" {
    // Stubs لتعطيل وظائف الـ OS
    char* getcwd(char* buf, size_t size) { return (char*)"/"; }
    int mkdir(const char* pathname, unsigned int mode) { return -1; }
    int dup(int oldfd) { return -1; }
    int dup2(int oldfd, int newfd) { return -1; }
    int regcomp(void* a, const char* b, int c) { return 0; }
    int regexec(const void* a, const char* b, size_t c, void* d, int e) { return 0; }
    void regfree(void* a) {}
}

#include <gtest/gtest.h>

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

// تعريف الـ main بصيغة C عشان الـ Linker يشوفها فوراً
extern "C" int main(int argc, char **argv) {
    printf("Starting RVV Vector Logic Validation...\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
EOF
