#include "pico/stdlib.h"

void delay(void)
{
    for (volatile uint32_t i = 0; i < 1000000; i++)
    {
    }
}

int main()
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1)
    {
        gpio_put(LED_PIN, 1);

        delay();

        gpio_put(LED_PIN, 0);

        delay();
    }
}