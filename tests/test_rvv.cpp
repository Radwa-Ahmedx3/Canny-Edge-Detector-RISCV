#include <riscv_vector.h>
#include <stdio.h>

int main() {
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8};
    int b[] = {10, 20, 30, 40, 50, 60, 70, 80};
    int result[8] = {0};
    int n = 8;

    for (int i = 0; i < n; ) {
        size_t vl = __riscv_vsetvl_e32m1(n - i);
        vint32m1_t va = __riscv_vle32_v_i32m1(a + i, vl);
        vint32m1_t vb = __riscv_vle32_v_i32m1(b + i, vl);
        vint32m1_t vc = __riscv_vadd_vv_i32m1(va, vb, vl);
        __riscv_vse32_v_i32m1(result + i, vc, vl);
        i += vl;
    }

    for (int i = 0; i < n; i++)
        printf("%d ", result[i]);
    printf("\n");
    return 0;
}
