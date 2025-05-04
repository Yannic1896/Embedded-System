#include "ses_button.h"
#include <util/delay.h>
#include <avr/io.h>
#include <ses_led.h>
#include <stdio.h>

int main(void) {
    // Initialize the button library
    button_init();

    // Variable to track time since boot in milliseconds
    uint32_t timeSinceBoot = 0;

    // Main loop
    while (1) {
        // Check if the push button is pressed
        if (button_isPushButtonPressed()) {
            led_greenOn();
        }

        // Check if the rotary button is pressed
        if (button_isRotaryButtonPressed()) {
            led_redOn();
        }

        // Add a small delay to debounce the buttons
        _delay_ms(500);

        // Increment time since boot (500 ms per loop iteration)
        timeSinceBoot += 500;

        // Print the time since boot in seconds
        printf("Time since boot: %lu ms\n", timeSinceBoot);
    }

    return 0;
}