#include <stdint.h>

// Declare GPIO function (imported from ESP32 firmware)
__attribute__((import_module("env"), import_name("gpio_set_level")))
void gpio_set_level(int pin, int level);

__attribute__((import_module("env"), import_name("sleep_ms")))
void sleep_ms(int milliseconds);

__attribute__((import_module("env"), import_name("print_debug")))
void print_debug(const char *message);

#define LED_GPIO 8  // On-board RGB LED is connected to GPIO 8

void main() {
    while (1) {
        print_debug("Turning LED ON");
        gpio_set_level(LED_GPIO, 1);  // Turn LED ON
        sleep_ms(1000);               // Delay 1 second

        print_debug("Turning LED OFF");
        gpio_set_level(LED_GPIO, 0);  // Turn LED OFF
        sleep_ms(1000);               // Delay 1 second
    }
}
