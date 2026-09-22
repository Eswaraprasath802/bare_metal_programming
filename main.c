#include <stdio.h>

int main(void)
{
    int x = 10;
    int y;

    asm volatile(
        "mov %[out], %[in]"
        : [out] "=r"(y)
        : [in] "r"(x)
    );

    printf("y = %d\n", y);

    return 0;
}