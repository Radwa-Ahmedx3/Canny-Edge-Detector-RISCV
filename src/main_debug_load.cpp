#include "image.hpp"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int main() {
    const char* path = "/home/dana_mk/Canny-Edge-Detector-RISCV/test_input.raw";
    
    std::cout << "Opening: " << path << "\n";
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        std::cout << "Error: fd=" << fd << "\n";
        return 1;
    }
    std::cout << "Opened, fd=" << fd << "\n";
    
    std::cout << "Creating 64x64 image...\n";
    Image img(64, 64);
    
    std::cout << "Reading...\n";
    int n = read(fd, img.getData(), 64 * 64);
    std::cout << "Read " << n << " bytes\n";
    
    close(fd);
    std::cout << "Success!\n";
    return 0;
}
