<<<<<<< HEAD
#include "ses_button.h"
#include "ses_led.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Function to toggle the red LED
void toggleRedLed(void) {
    led_redToggle(); // Toggle the red LED
}

// Function to toggle the green LED
void toggleGreenLed(void) {
    led_greenToggle(); // Toggle the green LED
}

int main(void) {
    // Initialize LEDs
    led_redInit();
    led_greenInit();
    sei();

    // Turn off LEDs initially
    led_redOff();
    led_greenOff();

    // Initialize buttons
    button_init();

    // Set button callbacks
    button_setRotaryButtonCallback(toggleRedLed);   // Rotary button toggles red LED
    button_setPushButtonCallback(toggleGreenLed);  // Push button toggles green LED

    // Main loop
    while (1) {
        
        // The program relies on interrupts to handle button presses
        // No additional code is needed in the main loop
        _delay_ms(500);

=======
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ses_button.c"
#include "ses_led.c"
#include <util/delay.h>
#include "ses_usbserial.h"

void onPushButtonPressed(void) {
    led_redToggle();
}

void onRotaryButtonPressed(void) {
    led_greenToggle();
}

int main(void) {
    led_redInit();
    led_greenInit();
    button_init(false);

    sei();

    button_setPushButtonCallback(onPushButtonPressed);
    button_setRotaryButtonCallback(onRotaryButtonPressed);

    while(1){
        _delay_ms(100);
>>>>>>> Jannis
    }

    return 0;
}
<<<<<<< HEAD

=======
>>>>>>> Jannis
