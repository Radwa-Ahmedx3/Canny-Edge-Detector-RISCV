#include <iostream>
#include "../include/image.hpp"
#include <cstdint>

int main() {
    std::cout << "--- Phase 1.5: RISC-V Infrastructure Test ---" << std::endl;
    
    Image img(128, 128);
    uintptr_t addr = reinterpret_cast<uintptr_t>(img.getData());
    
    std::cout << "Allocated Address: 0x" << std::hex << addr << std::dec << std::endl;

    if (addr % 64 == 0 && addr != 0) {
        std::cout << "[SUCCESS] Memory Alignment Verified (64-byte)." << std::endl;
        std::cout << "[SUCCESS] QEMU can execute the compiled Binary." << std::endl;
    } else {
        std::cout << "[FAILED] Alignment issue or allocation failed." << std::endl;
    }

    return 0;
}
