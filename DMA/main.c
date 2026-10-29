
#include <stdint.h>

/* Direct 32-bit access to a hardware register */
#define REG32(addr) (*(volatile uint32_t *)(addr))

#define DMA_BASE           0x50000000u

/* DMA channel 0 registers */
#define DMA_CH0_READ_ADDR  REG32(DMA_BASE + 0x00u)
#define DMA_CH0_WRITE_ADDR REG32(DMA_BASE + 0x04u)
#define DMA_CH0_TRANS_COUNT REG32(DMA_BASE + 0x08u)
#define DMA_CH0_CTRL_TRIG  REG32(DMA_BASE + 0x0Cu)

/* CTRL_TRIG bit settings */
#define DMA_EN             (1u << 0)
#define DMA_DATA_SIZE_32   (2u << 2)
#define DMA_INCR_WRITE     (1u << 5)
#define DMA_TREQ_PERMANENT (0x3Fu << 15)

/* Source and destination buffers in RAM */
static volatile uint32_t source[3] = {
    100u, 200u, 300u
};

static volatile uint32_t destination[3] = {
    0u, 0u, 0u
};

int main(void)
{
    /* 1. Set the source address */
    DMA_CH0_READ_ADDR =
        (uint32_t)(uintptr_t)&source[0];

    /* 2. Set the destination address */
    DMA_CH0_WRITE_ADDR =
        (uint32_t)(uintptr_t)&destination[0];

    /* 3. Request three 32-bit transfers */
    DMA_CH0_TRANS_COUNT = 3u;

    /* 4. Configure DMA and start the transfer */
    DMA_CH0_CTRL_TRIG =
        DMA_EN |
        DMA_DATA_SIZE_32 |
        DMA_INCR_WRITE |
        DMA_TREQ_PERMANENT;

    /* 5. Wait until all transfers finish */
    while (DMA_CH0_TRANS_COUNT != 0u) {
        /* DMA hardware performs the copying */
    }

    /* 6. Verify the result */
    if (destination[0] == 100u &&
        destination[1] == 200u &&
        destination[2] == 300u) {
        return 0; /* Success */
    }

    return 1;     /* Copy failed */
}
