#include <stdint.h>

#define GPIO_OUT_SET (*(volatile uint32_t *)0xD0000014)

int main(void)
{
    __asm__ volatile (
        "movs r0, #1 \n"
        "ldr  r1, =0xD0000014 \n"
        "str  r0, [r1] \n"
        :
        :
        : "r0", "r1", "memory"
    );

    while (1);
}