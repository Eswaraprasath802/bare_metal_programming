#include <stdint.h>

/* Register access */
#define REG32(addr) (*(volatile uint32_t *)(addr))

/* Reset controller */
#define RESETS_BASE       0x4000C000u
#define RESETS_RESET      REG32(RESETS_BASE + 0x00u)
#define RESETS_RESET_DONE REG32(RESETS_BASE + 0x08u)

/* GPIO control registers */
#define IO_BANK0_BASE 0x40014000u
#define GPIO2_CTRL REG32(IO_BANK0_BASE + 0x0Cu)
#define GPIO3_CTRL REG32(IO_BANK0_BASE + 0x10u)
#define GPIO4_CTRL REG32(IO_BANK0_BASE + 0x14u)
#define GPIO5_CTRL REG32(IO_BANK0_BASE + 0x18u)

/* SIO registers */
#define SIO_BASE         0xD0000000u
#define GPIO_OUT_SET     REG32(SIO_BASE + 0x14u)
#define GPIO_OUT_CLR     REG32(SIO_BASE + 0x18u)
#define GPIO_OE_SET      REG32(SIO_BASE + 0x24u)

#define CS_MASK (1u << 5)

/* SPI0 registers */
#define SPI0_BASE 0x4003C000u
#define SPI0_CR0  REG32(SPI0_BASE + 0x00u)
#define SPI0_CR1  REG32(SPI0_BASE + 0x04u)
#define SPI0_DR   REG32(SPI0_BASE + 0x08u)
#define SPI0_SR   REG32(SPI0_BASE + 0x0Cu)
#define SPI0_CPSR REG32(SPI0_BASE + 0x10u)

/* SPI status bits */
#define SPI_BSY (1u << 4)
#define SPI_RNE (1u << 2)
#define SPI_TNF (1u << 1)

static void spi0_init(void)
{
    /* Release SPI0 from reset. */
    RESETS_RESET &= ~(1u << 16);

    while ((RESETS_RESET_DONE & (1u << 16)) == 0u) {
    }

    /* GP2=SCK, GP3=TX, GP4=RX: SPI0 function. */
    GPIO2_CTRL = 1u;
    GPIO3_CTRL = 1u;
    GPIO4_CTRL = 1u;

    /* GP5 = normal SIO GPIO for chip select. */
    GPIO5_CTRL = 5u;
    GPIO_OUT_SET = CS_MASK;
    GPIO_OE_SET = CS_MASK;

    /* Disable SPI while configuring it. */
    SPI0_CR1 = 0u;

    /* 8-bit transfers, SPI Mode 0. */
    SPI0_CR0 = 7u;

    /* Even prescaler: 254. */
    SPI0_CPSR = 254u;

    /* Enable SPI; master mode is the reset default. */
    SPI0_CR1 = (1u << 1);
}

static uint8_t spi_transfer(uint8_t tx)
{
    /* Wait for transmit FIFO space. */
    while ((SPI0_SR & SPI_TNF) == 0u) {
    }

    /* Send one byte. Hardware generates the clock. */
    SPI0_DR = tx;

    /* Wait until a received byte is available. */
    while ((SPI0_SR & SPI_RNE) == 0u) {
    }

    return (uint8_t)SPI0_DR;
}

static uint16_t mcp3002_read_ch0(void)
{
    uint8_t high;
    uint8_t low;

    GPIO_OUT_CLR = CS_MASK;  /* Select ADC */

    /*
     * 0x68 = leading zero, start=1, single-ended=1,
     * channel 0=0, MSB-first=1, followed by don't-care bits.
     */
    high = spi_transfer(0x68u);
    low  = spi_transfer(0x00u);

    while ((SPI0_SR & SPI_BSY) != 0u) {
    }

    GPIO_OUT_SET = CS_MASK;  /* Finish transaction */

    /* Combine B9:B8 from high with B7:B0 from low. */
    return (uint16_t)(((uint16_t)(high & 0x03u) << 8) | low);
}

int main(void)
{
    volatile uint16_t adc_value;

    spi0_init();

    while (1) {
        adc_value = mcp3002_read_ch0();

        /* Inspect adc_value in the debugger.
           Expected range: 0 to 1023. */
    }
}