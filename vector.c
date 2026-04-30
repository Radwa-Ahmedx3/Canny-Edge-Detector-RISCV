#include <stdio.h>
#include <riscv_vector.h>

int main() {
    size_t vl = __riscv_vsetvl_e32m1(4);

    int a[4] = {1,2,3,4};
    int b[4] = {5,6,7,8};
    int c[4];

    vint32m1_t va = __riscv_vle32_v_i32m1(a, vl);
    vint32m1_t vb = __riscv_vle32_v_i32m1(b, vl);

    vint32m1_t vc = __riscv_vadd_vv_i32m1(va, vb, vl);

    __riscv_vse32_v_i32m1(c, vc, vl);

    for(int i=0;i<4;i++){
        printf("%d ", c[i]);
    }

    return 0;
}
