#include <stdint.h>

/* ---------- SIO ---------- */

#define SIO_BASE        0xD0000000u

#define GPIO_OUT        (*(volatile uint32_t *)(SIO_BASE + 0x10))
#define GPIO_OE         (*(volatile uint32_t *)(SIO_BASE + 0x20))


/* ---------- IO_BANK0 ---------- */

#define IO_BANK0_BASE   0x40014000u

/*
 * Each GPIO has an 8-byte register block.
 *
 * GPIO25_CTRL:
 * 0x40014000 + (25 * 8) + 4
 * = 0x400140CC
 */
#define GPIO25_CTRL     (*(volatile uint32_t *)(IO_BANK0_BASE + 0xCC))


/* ---------- TIMER ---------- */

#define TIMER_BASE      0x40054000u

#define TIMERAWL        (*(volatile uint32_t *)(TIMER_BASE + 0x28))


/* ---------- GPIO ---------- */

#define LED_PIN         25u

/* FUNCSEL = 5 means SIO */
#define GPIO_FUNC_SIO   5u


static void gpio25_init(void)
{
    /*
     * Configure GPIO25 to use the SIO peripheral.
     *
     * FUNCSEL bits = bits 0..4
     */
    GPIO25_CTRL = GPIO_FUNC_SIO;

    /*
     * Enable GPIO25 as an output.
     */
    GPIO_OE |= (1u << LED_PIN);
}


static void led_on(void)
{
    GPIO_OUT |= (1u << LED_PIN);
}


static void led_off(void)
{
    GPIO_OUT &= ~(1u << LED_PIN);
}


static void delay_1_second(void)
{
    uint32_t start = TIMERAWL;

    while ((uint32_t)(TIMERAWL - start) < 1000000u)
    {
        /* Wait */
    }
}


int main(void)
{
    gpio25_init();

    while (1)
    {
        led_on();

        delay_1_second();

        led_off();

        delay_1_second();
    }

    return 0;
}