#include "pico/stdlib.h"

int main()
{
  stdio_init_all();
    // Initialize the onboard LED GPIO
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Record the starting hardware timer value
    uint64_t start_time = time_us_64();

    while (1)
    {
        // Read the hardware timer
        uint64_t current_time = time_us_64();

        // Check whether 1 second has elapsed
        if ((current_time - start_time) >= 1000000)
        {
            // Toggle LED
            gpio_put(LED_PIN, !gpio_get(LED_PIN));

            // Start measuring the next 1-second interval
            start_time = current_time;
        }
    }
}