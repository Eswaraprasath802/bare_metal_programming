#include <stdint.h>

#define RESETS_BASE       0x4000C000u
#define RESETS_RESET      (*(volatile uint32_t *)(RESETS_BASE + 0x00u))
#define RESETS_RESET_DONE (*(volatile uint32_t *)(RESETS_BASE + 0x08u))

#define IO_BANK0_BASE     0x40014000u
#define GPIO0_CTRL        (*(volatile uint32_t *)(IO_BANK0_BASE + 0x04u))

#define PADS_BANK0_BASE   0x4001C000u
#define GPIO0_PAD         (*(volatile uint32_t *)(PADS_BANK0_BASE + 0x04u))

#define SIO_BASE          0xD0000000u
#define GPIO_OUT_SET      (*(volatile uint32_t *)(SIO_BASE + 0x014u))
#define GPIO_OUT_CLR      (*(volatile uint32_t *)(SIO_BASE + 0x018u))
#define GPIO_OE_SET       (*(volatile uint32_t *)(SIO_BASE + 0x024u))

#define GPIO_FUNC_SIO     5u
#define GPIO0_MASK        (1u << 0)

static void delay(void)
{
    for (volatile uint32_t i = 0; i < 500000u; i++)
    {
        __asm volatile ("nop");
    }
}

int main(void)
{
    /* Release GPIO bank from reset */
    RESETS_RESET &= ~(1u << 5);

    while ((RESETS_RESET_DONE & (1u << 5)) == 0u)
    {
    }

    /* Configure GPIO 0 for SIO */
    GPIO0_CTRL = GPIO_FUNC_SIO;

    /* Enable output */
    GPIO_OE_SET = GPIO0_MASK;

    while (1)
    {
        GPIO_OUT_SET = GPIO0_MASK;
        delay();

        GPIO_OUT_CLR = GPIO0_MASK;
        delay();
    }
}