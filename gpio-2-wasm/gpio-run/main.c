#include <stdint.h>

__attribute__((import_module("env"), import_name("gpio_set_level")))
void gpio_set_level(int pin, int level);

#define GPIO_PIN 2

void main() {
    gpio_set_level(GPIO_PIN, 1);  
    for (volatile int i = 0; i < 1000000; i++);  
    gpio_set_level(GPIO_PIN, 0);  
}
