#include <stdint.h>

/* ---------- Register access ---------- */
#define REG32(addr) (*(volatile uint32_t *)(addr))

/* RP2040 I2C0 registers */
#define I2C0_BASE       0x40044000u
#define IC_CON          REG32(I2C0_BASE + 0x00)
#define IC_TAR          REG32(I2C0_BASE + 0x04)
#define IC_DATA_CMD     REG32(I2C0_BASE + 0x10)
#define IC_SS_SCL_HCNT  REG32(I2C0_BASE + 0x14)
#define IC_SS_SCL_LCNT  REG32(I2C0_BASE + 0x18)
#define IC_RAW_INTR_STAT REG32(I2C0_BASE + 0x34)
#define IC_CLR_TX_ABRT  REG32(I2C0_BASE + 0x54)
#define IC_ENABLE       REG32(I2C0_BASE + 0x6C)
#define IC_STATUS       REG32(I2C0_BASE + 0x70)
#define IC_TXFLR        REG32(I2C0_BASE + 0x74)
#define IC_TX_ABRT_SOURCE REG32(I2C0_BASE + 0x80)

/* Reset controller */
#define RESETS_BASE     0x4000C000u
#define RESET_REG       REG32(RESETS_BASE + 0x00)
#define RESET_DONE      REG32(RESETS_BASE + 0x08)

/* GPIO function select */
#define IO_BANK0_BASE   0x40014000u
#define GPIO4_CTRL      REG32(IO_BANK0_BASE + 0x24)
#define GPIO5_CTRL      REG32(IO_BANK0_BASE + 0x2C)

/* GPIO pad control */
#define PADS_BANK0_BASE 0x4001C000u
#define GPIO4_PAD       REG32(PADS_BANK0_BASE + 0x14)
#define GPIO5_PAD       REG32(PADS_BANK0_BASE + 0x18)

/* ---------- LCD settings ---------- */
#define LCD_ADDR        0x27u
#define LCD_BACKLIGHT   0x08u
#define LCD_ENABLE      0x04u
#define LCD_RS          0x01u

/* ---------- Delays ---------- */
static void delay_short(void)
{
    for (volatile uint32_t i = 0; i < 500; i++) {
        __asm volatile ("nop");
    }
}

static void delay_ms(uint32_t ms)
{
    while (ms--) {
        for (volatile uint32_t i = 0; i < 12500; i++) {
            __asm volatile ("nop");
        }
    }
}

/* ---------- I2C initialization ---------- */
static void i2c_init(void)
{
    /* Reset I2C0 */
    RESET_REG |= (1u << 3);
    RESET_REG &= ~(1u << 3);

    while ((RESET_DONE & (1u << 3)) == 0) {
    }

    /* GP4 = SDA, GP5 = SCL; function 3 = I2C */
    GPIO4_CTRL = 3;
    GPIO5_CTRL = 3;

    /* Enable input and pull-ups */
    GPIO4_PAD = (1u << 6) | (1u << 3) | (1u << 1);
    GPIO5_PAD = (1u << 6) | (1u << 3) | (1u << 1);

    IC_ENABLE = 0;

    /* Master, standard speed, restart enabled, slave disabled */
    IC_CON = (1u << 0) |
             (1u << 1) |
             (1u << 5) |
             (1u << 6);

    /* Approximate 100 kHz timing for a 125 MHz system clock */
    IC_SS_SCL_HCNT = 500;
    IC_SS_SCL_LCNT = 600;

    IC_TAR = LCD_ADDR;
    IC_ENABLE = 1;
}

/* ---------- Send one byte to I2C backpack ---------- */
static int i2c_write(uint8_t value)
{
    uint32_t timeout = 1000000;

    /* Wait until the transmit FIFO can accept a byte */
    while ((IC_STATUS & (1u << 1)) == 0) {
        if (timeout-- == 0)
            return -1;

        if (IC_RAW_INTR_STAT & (1u << 6)) {
            (void)IC_CLR_TX_ABRT;
            return -1;
        }
    }

    /* Queue data byte and STOP */
    IC_DATA_CMD = 0x200u | value;

    timeout = 1000000;

    /* Wait until transmit FIFO is empty */
    while (IC_TXFLR != 0) {
        if (timeout-- == 0)
            return -1;

        if (IC_RAW_INTR_STAT & (1u << 6)) {
            (void)IC_TX_ABRT_SOURCE;
            (void)IC_CLR_TX_ABRT;
            return -1;
        }
    }

    /* Wait until the I2C master is no longer active */
    timeout = 1000000;

    while (IC_STATUS & (1u << 5)) {
        if (timeout-- == 0)
            return -1;

        if (IC_RAW_INTR_STAT & (1u << 6)) {
            (void)IC_TX_ABRT_SOURCE;
            (void)IC_CLR_TX_ABRT;
            return -1;
        }
    }

    if (IC_RAW_INTR_STAT & (1u << 6)) {
        (void)IC_TX_ABRT_SOURCE;
        (void)IC_CLR_TX_ABRT;
        return -1;
    }

    return 0;
}

/* ---------- LCD backpack output ---------- */
static int lcd_expander_write(uint8_t value)
{
    /* Keep backlight enabled on every write */
    return i2c_write(value | LCD_BACKLIGHT);
}

static int lcd_pulse_enable(uint8_t value)
{
    if (lcd_expander_write(value | LCD_ENABLE) != 0)
        return -1;

    delay_short();

    if (lcd_expander_write(value & ~LCD_ENABLE) != 0)
        return -1;

    delay_short();
    return 0;
}

static int lcd_send_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t value = (nibble & 0xF0u) | rs | LCD_BACKLIGHT;
    return lcd_pulse_enable(value);
}

static int lcd_send_byte(uint8_t value, uint8_t rs)
{
    if (lcd_send_nibble(value & 0xF0u, rs) != 0)
        return -1;

    if (lcd_send_nibble((value << 4) & 0xF0u, rs) != 0)
        return -1;

    return 0;
}

static int lcd_command(uint8_t command)
{
    int result = lcd_send_byte(command, 0);

    if (command == 0x01 || command == 0x02)
        delay_ms(3);
    else
        delay_ms(1);

    return result;
}

static int lcd_data(uint8_t data)
{
    return lcd_send_byte(data, LCD_RS);
}

static int lcd_init(void)
{
    delay_ms(50);

    /* Standard HD44780 4-bit initialization sequence */
    if (lcd_send_nibble(0x30, 0) != 0) return -1;
    delay_ms(5);

    if (lcd_send_nibble(0x30, 0) != 0) return -1;
    delay_ms(2);

    if (lcd_send_nibble(0x30, 0) != 0) return -1;
    delay_ms(2);

    if (lcd_send_nibble(0x20, 0) != 0) return -1;
    delay_ms(2);

    if (lcd_command(0x28) != 0) return -1; /* 4-bit, 2-line */
    if (lcd_command(0x0C) != 0) return -1; /* Display on */
    if (lcd_command(0x06) != 0) return -1; /* Cursor moves right */
    if (lcd_command(0x01) != 0) return -1; /* Clear display */

    return 0;
}

int main(void)
{
    i2c_init();

    /* Set backlight on before initializing the LCD */
    if (lcd_expander_write(LCD_BACKLIGHT) != 0)
        goto error;

    if (lcd_init() != 0)
        goto error;

    /* Display HELLO */
    if (lcd_data('H') != 0) goto error;
    if (lcd_data('E') != 0) goto error;
    if (lcd_data('L') != 0) goto error;
    if (lcd_data('L') != 0) goto error;
    if (lcd_data('O') != 0) goto error;

    while (1) {
    }

error:
    /* Stop I2C and leave the program here for debugging */
    IC_ENABLE = 0;

    while (1) {
    }
}