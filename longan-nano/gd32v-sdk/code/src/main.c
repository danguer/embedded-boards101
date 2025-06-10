/*
this was adapted from SDK/Examples/GPIO/Running_led
to use details for longan nano board
*/

#include "gd32vf103.h"
#include "systick.h"
#include <stdio.h>

int main(void)
{
    uint8_t led_color = 1;
    // GPIO PC13=Red Led, PA1=Green, PA2=Blue

    /* enable the LED clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure LED GPIO port */
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    // turn off led
    gpio_bit_set(GPIOC, GPIO_PIN_13);
    gpio_bit_set(GPIOA, GPIO_PIN_1);
    gpio_bit_set(GPIOA, GPIO_PIN_2);

    while(1) {
        // turn red led
        if (led_color == 0x4)
            gpio_bit_reset(GPIOC, GPIO_PIN_13);

        // turn green led
        if (led_color == 0x2)
            gpio_bit_reset(GPIOA, GPIO_PIN_1);

        // turn blue led
        if (led_color == 0x1)
            gpio_bit_reset(GPIOA, GPIO_PIN_2);

        delay_1ms(1000);

        /* turn off LED */
        gpio_bit_set(GPIOC, GPIO_PIN_13);
        gpio_bit_set(GPIOA, GPIO_PIN_1);
        gpio_bit_set(GPIOA, GPIO_PIN_2);
        delay_1ms(1000);

        if (led_color < 0x4) {
            led_color <<= 1;
        } else {
            led_color = 1;
        }
    }
}
