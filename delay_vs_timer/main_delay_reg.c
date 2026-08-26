#include <stdint.h>

#define LED_PIN 25

#define SIO_GPIO_OUT_SET (*(volatile uint32_t *)0xD0000014)
#define SIO_GPIO_OUT_CLR (*(volatile uint32_t *)0xD0000018)
#define SIO_GPIO_OE_SET  (*(volatile uint32_t *)0xD0000024)

void delay(void)
{
    for (volatile uint32_t i = 0; i < 1000000; i++)
    {
    }
}

int main(void)
{
    // GPIO25 as output
    SIO_GPIO_OE_SET = (1u << LED_PIN);

    while (1)
    {
        // LED ON
        SIO_GPIO_OUT_SET = (1u << LED_PIN);

        delay();

        // LED OFF
        SIO_GPIO_OUT_CLR = (1u << LED_PIN);

        delay();
    }
}