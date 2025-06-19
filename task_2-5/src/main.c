#include "ses_button.h"
#include "ses_led.h" // Include the LED library for controlling LEDs
#include "ses_usbserial.h"
#include "ses_display.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

int main(void) {
    // Initialize the button library
    button_init();
    led_redInit(); // Initialize red LED
    led_yellowInit(); // Initialize yellow LED
    led_greenInit(); // Initialize green LED
    
    // Main loop
    while (1) {
        // Check if the push button is pressed
    led_greenOff(); // Turn off green LED initially
    led_yellowOff(); // Turn off yellow LED initially
    led_redOff(); // Turn off red LED initially

        if (button_isPushButtonPressed()) {
            // Perform an action when the push button is pressed
            led_greenOn();// Example action: turn on yellow LED
            _delay_ms(1000); // Keep the LED on for 1 second
        }

        // Check if the rotary button is pressed
        if (button_isRotaryButtonPressed()) {
            led_yellowOn(); // Example action: turn on yellow LED
            _delay_ms(1000); // Keep the LED on for 1 second
        }

        // Add a small delay to debounce the buttons
        _delay_ms(50);
    }

    return 0;
}