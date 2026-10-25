#include <stdint.h>

/* Reset registers */
#define RESETS_RESET       (*(volatile uint32_t *)0x4000C000)
#define RESETS_RESET_DONE  (*(volatile uint32_t *)0x4000C008)

/* GPIO 2 control register */
#define GPIO2_CTRL         (*(volatile uint32_t *)0x4001400C)

/* PWM slice 1 registers */
#define PWM_CH1_CSR        (*(volatile uint32_t *)0x40050014)
#define PWM_CH1_DIV        (*(volatile uint32_t *)0x40050018)
#define PWM_CH1_CC         (*(volatile uint32_t *)0x40050020)
#define PWM_CH1_TOP        (*(volatile uint32_t *)0x40050024)

int main(void)
{
    /* 1. Release PWM from reset */
    RESETS_RESET &= ~(1u << 14);

    /* 2. Wait until PWM is ready */
    while ((RESETS_RESET_DONE & (1u << 14)) == 0)
    {
    }

    /* 3. Connect GPIO 2 to PWM function */
    GPIO2_CTRL = (GPIO2_CTRL & ~0x1Fu) | 4u;

    /* 4. Set PWM clock divider to 2 */
    PWM_CH1_DIV = 2u << 4;

    /* 5. Set PWM cycle length */
    PWM_CH1_TOP = 62499u;

    /* 6. Set duty cycle to approximately 50% */
    PWM_CH1_CC = 31250u;

    /* 7. Enable PWM slice 1 */
    PWM_CH1_CSR = 1u;

    while (1)
    {
    }
}