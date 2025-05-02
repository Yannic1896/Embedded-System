#include "ses_button.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

int main(void) {
    // Initialize the button library
    button_init();

    // Main loop
    while (1) {
        // Check if the push button is pressed
        if (button_isPushButtonPressed()) {
            // Perform an action when the push button is pressed
            // Example: Toggle an LED or send a debug message
            printf("Push button pressed!\n");
        }

        // Check if the rotary button is pressed
        if (button_isRotaryButtonPressed()) {
            // Perform an action when the rotary button is pressed
            // Example: Toggle an LED or send a debug message
            printf("Rotary button pressed!\n");
        }

        // Add a small delay to debounce the buttons
        _delay_ms(50);
    }

    return 0;
}