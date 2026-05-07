#include <iostream>
#include <riscv_vector.h>

int main() {

    size_t vl = 4;

    int32_t a[4] = {1,2,3,4};
    int32_t b[4] = {10,20,30,40};
    int32_t c[4];

    vint32m1_t va = __riscv_vle32_v_i32m1(a, vl);
    vint32m1_t vb = __riscv_vle32_v_i32m1(b, vl);

    vint32m1_t vc = __riscv_vadd_vv_i32m1(va, vb, vl);

    __riscv_vse32_v_i32m1(c, vc, vl);

    for(int i = 0; i < 4; i++) {
        std::cout << c[i] << " ";
    }

    std::cout << std::endl;

    return 0;
}
