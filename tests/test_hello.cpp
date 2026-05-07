#include <stddef.h>
#include <stdio.h>


extern "C" {
    char* getcwd(char* buf, size_t size) {
        if (buf && size > 1) {
            buf[0] = '/';
            buf[1] = '\0';
            return buf;
        }
        return (char*)"/";
    }
    int mkdir(const char* pathname, unsigned int mode) { return -1; }
    int dup(int oldfd) { return -1; }
    int dup2(int oldfd, int newfd) { return -1; }
    int regcomp(void* a, const char* b, int c) { return 0; }
    int regexec(const void* a, const char* b, size_t c, void* d, int e) { return 0; }
    void regfree(void* a) {}
}

#include <gtest/gtest.h>

TEST(CannyInfra, BasicCheck) {
    EXPECT_EQ(1, 1);
}

int main(int argc, char **argv) {
    printf("Starting Canny Edge Detector Infrastructure Test...\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
