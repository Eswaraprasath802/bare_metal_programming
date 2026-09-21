#include <stdint.h>

/*
 * ============================================================
 * UART0 REGISTERS
 * ============================================================
 */

#define UART0_BASE      0x40034000u

#define UART0_DR        (*(volatile uint32_t *)(UART0_BASE + 0x000))
#define UART0_FR        (*(volatile uint32_t *)(UART0_BASE + 0x018))
#define UART0_IBRD      (*(volatile uint32_t *)(UART0_BASE + 0x024))
#define UART0_FBRD      (*(volatile uint32_t *)(UART0_BASE + 0x028))
#define UART0_LCR_H     (*(volatile uint32_t *)(UART0_BASE + 0x02C))
#define UART0_CR        (*(volatile uint32_t *)(UART0_BASE + 0x030))


/*
 * ============================================================
 * GPIO REGISTERS
 * ============================================================
 */

#define IO_BANK0_BASE   0x40014000u

#define GPIO0_CTRL      (*(volatile uint32_t *)(IO_BANK0_BASE + 0x004))
#define GPIO1_CTRL      (*(volatile uint32_t *)(IO_BANK0_BASE + 0x00C))

/*
 * GPIO function 2 = UART0
 */
#define GPIO_FUNC_UART  2u


/*
 * ============================================================
 * UART INITIALIZATION
 * ============================================================
 */

static void uart0_init(void)
{
    /*
     * GPIO0 = UART0 TX
     * GPIO1 = UART0 RX
     */
    GPIO0_CTRL = GPIO_FUNC_UART;
    GPIO1_CTRL = GPIO_FUNC_UART;


    /*
     * Disable UART before configuration
     */
    UART0_CR = 0;


    /*
     * UART clock = 125 MHz
     *
     * Baud rate = 9600
     *
     * UART divisor:
     *
     * 125000000 / (16 * 9600)
     * = 813.802...
     *
     * Integer part = 813
     * Fractional part ≈ 51
     */

    UART0_IBRD = 813;
    UART0_FBRD = 51;


    /*
     * Line control:
     *
     * 8 data bits
     * 1 stop bit
     * No parity
     */
    UART0_LCR_H = (3u << 5);


    /*
     * Enable:
     *
     * bit 0 = UART enable
     * bit 8 = transmit enable
     * bit 9 = receive enable
     */
    UART0_CR =
        (1u << 0) |
        (1u << 8) |
        (1u << 9);
}


/*
 * ============================================================
 * SEND ONE CHARACTER
 * ============================================================
 */

static void uart0_putc(char c)
{
    /*
     * Wait while transmit FIFO is full.
     *
     * UART0_FR bit 5 = TXFF
     */
    while (UART0_FR & (1u << 5))
    {
    }

    /*
     * Write character to UART data register
     */
    UART0_DR = (uint32_t)c;
}


/*
 * ============================================================
 * SEND STRING
 * ============================================================
 */

static void uart0_puts(const char *str)
{
    while (*str != '\0')
    {
        uart0_putc(*str);
        str++;
    }
}


/*
 * ============================================================
 * MAIN
 * ============================================================
 */

int main(void)
{
    uart0_init();

    while (1)
    {
        uart0_puts("Hello from Raspberry Pi Pico!\r\n");

        /*
         * Simple delay.
         */
        for (volatile uint32_t i = 0;
             i < 1000000;
             i++)
        {
        }
    }

    return 0;
}