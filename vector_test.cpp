#include <riscv_vector.h>
#include <stdio.h>
#include <stdint.h>

int main() {
    int32_t a[] = {1, 2, 3, 4};
    int32_t b[] = {10, 20, 30, 40};
    int32_t c[4] = {0};

    size_t vl = __riscv_vsetvl_e32m1(4);
    vint32m1_t va = __riscv_vle32_v_i32m1(a, vl);
    vint32m1_t vb = __riscv_vle32_v_i32m1(b, vl);
    vint32m1_t vc = __riscv_vadd_vv_i32m1(va, vb, vl);
    __riscv_vse32_v_i32m1(c, vc, vl);

    printf("Vector Result: %d %d %d %d\n", c[0], c[1], c[2], c[3]);
    return 0;
}
