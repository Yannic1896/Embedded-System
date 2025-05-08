#include "ses_button.h"
#include <util/delay.h>
#include <avr/io.h>
#include "ses_led.h"
#include <stdio.h>
#include "ses_usbserial.h"
#include "ses_display.h"

int main(void) {

    display_init(); // Initialize the display
    usbserial_init(); // Initialize USB serial communication
    // Initialize the button library
    button_init();
    led_redInit(); // Initialize red LED
    led_greenInit();

    led_redOff(); // Turn off red LED initially
    led_greenOff(); // Turn off green LED initially

    // Variable to track time since boot in milliseconds
    uint32_t timeSinceBoot = 0;

    // Main loop
    while (1) {
        // Check if the push button is pressed
        if (button_isPushButtonPressed()) {
            led_greenOn();
            _delay_ms(1000); // Keep the LED on for 1 second
            led_greenOff();
        }

        // Check if the rotary button is pressed
        if (button_isRotaryButtonPressed()) {
            led_redOn();
            _delay_ms(1000); // Keep the LED on for 1 second
            led_redOff(); // Turn off red LED after 1 second
        }
        display_setCursor(0,0); // Set cursor to column 0, row 0
        fprintf(displayout, "Time since boot: %lu ms\n", timeSinceBoot);
        // Add a small delay to debounce the buttons
        display_update(); // Update the display with the new content
        _delay_ms(500);

        // Increment time since boot (500 ms per loop iteration)
        timeSinceBoot += 500;
    }

    return 0;
}