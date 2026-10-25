#include <stdint.h>

/* GPIO function-control registers */
#define GPIO0_CTRL   (*(volatile uint32_t *)0x40014004u)
#define GPIO25_CTRL  (*(volatile uint32_t *)0x400140CCu)

/* GPIO pad-control register for GP0 */
#define GPIO0_PAD    (*(volatile uint32_t *)0x4001C004u)

/* SIO registers */
#define GPIO_IN      (*(volatile uint32_t *)0xD0000004u)
#define GPIO_OE_SET  (*(volatile uint32_t *)0xD0000024u)
#define GPIO_OE_CLR  (*(volatile uint32_t *)0xD0000028u)
#define GPIO_OUT_SET (*(volatile uint32_t *)0xD0000014u)
#define GPIO_OUT_CLR (*(volatile uint32_t *)0xD0000018u)

int main(void)
{
    /* Configure GP0 as SIO */
    GPIO0_CTRL = 5u;

    /* Enable the GP0 input path */
    GPIO0_PAD |= (1u << 6);

    /* Disable output driver for GP0 */
    GPIO_OE_CLR = (1u << 0);

    /* Configure onboard LED GP25 as SIO output */
    GPIO25_CTRL = 5u;
    GPIO_OE_SET = (1u << 25);

    while (1)
    {
        /* Read only GP0's input bit */
        uint32_t sensor = (GPIO_IN >> 0u) & 1u;

        if (sensor == 0u)
        {
            /* Sensor output LOW: LED ON */
            GPIO_OUT_SET = (1u << 25);
        }
        else
        {
            /* Sensor output HIGH: LED OFF */
            GPIO_OUT_CLR = (1u << 25);
        }
    }
}