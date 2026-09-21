#include <stdint.h>

/*
 * RP2040 UART0
 */
#define UART0_BASE      0x40034000u

#define UART0_DR        (*(volatile uint32_t *)(UART0_BASE + 0x000))
#define UART0_FR        (*(volatile uint32_t *)(UART0_BASE + 0x018))
#define UART0_IBRD      (*(volatile uint32_t *)(UART0_BASE + 0x024))
#define UART0_FBRD      (*(volatile uint32_t *)(UART0_BASE + 0x028))
#define UART0_LCR_H     (*(volatile uint32_t *)(UART0_BASE + 0x02C))
#define UART0_CR        (*(volatile uint32_t *)(UART0_BASE + 0x030))


/*
 * IO_BANK0
 */
#define IO_BANK0_BASE   0x40014000u

#define GPIO0_CTRL      (*(volatile uint32_t *)(IO_BANK0_BASE + 0x004))
#define GPIO1_CTRL      (*(volatile uint32_t *)(IO_BANK0_BASE + 0x00C))


/*
 * GPIO function 2 = UART
 */
#define GPIO_FUNC_UART  2u


/*
 * Initialize UART0
 *
 * UART clock = 125 MHz
 * Baud rate = 9600
 */
static void uart0_init(void)
{
    /*
     * GPIO0 -> UART0 TX
     * GPIO1 -> UART0 RX
     */
    GPIO0_CTRL = GPIO_FUNC_UART;
    GPIO1_CTRL = GPIO_FUNC_UART;

    /*
     * Disable UART while configuring it.
     */
    UART0_CR = 0;

    /*
     * Baud rate divisor:
     *
     * UART baud divisor =
     * UART clock / (16 * baud rate)
     *
     * 125000000 / (16 * 9600)
     * = 813.802...
     *
     * Integer part = 813
     * Fractional part ≈ 0.802
     *
     * Fractional register:
     * 0.802 * 64 ≈ 51
     */
    UART0_IBRD = 813;
    UART0_FBRD = 51;

    /*
     * LCR_H:
     *
     * WLEN = 3 -> 8 data bits
     * STP2 = 0 -> 1 stop bit
     * PEN  = 0 -> no parity
     *
     * Bits [6:5] = 11
     */
    UART0_LCR_H = (3u << 5);

    /*
     * CR:
     *
     * Bit 0 = UART enable
     * Bit 8 = TX enable
     * Bit 9 = RX enable
     */
    UART0_CR =
        (1u << 0) |
        (1u << 8) |
        (1u << 9);
}


/*
 * Send one character
 */
static void uart0_putc(char c)
{
    /*
     * FR bit 5 = TXFF
     *
     * 1 = TX FIFO full
     * 0 = space available
     */
    while (UART0_FR & (1u << 5))
    {
    }

    UART0_DR = (uint32_t)c;
}


/*
 * Send a string
 */
static void uart0_puts(const char *str)
{
    while (*str != '\0')
    {
        uart0_putc(*str);
        str++;
    }
}


int main(void)
{
    uart0_init();

    while (1)
    {
        uart0_puts("Hello from Pico W!\r\n");

        /*
         * Simple delay
         */
        for (volatile uint32_t i = 0; i < 1000000; i++)
        {
        }
    }

    return 0;
}