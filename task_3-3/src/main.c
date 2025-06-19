#include "ses_button.h"
#include "ses_led.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void toggleRedLED(void) {
    led_redToggle();
}
void toggleGreenLED(void) {
    led_greenToggle();
}

int main(void) {

    // Initialize buttons
    button_init(false);

    // Set button callbacks
    button_setRotaryButtonCallback(toggleRedLED);   // Rotary button toggles red LED
    button_setPushButtonCallback(toggleGreenLED);  // Push button toggles green LED

    // Main loop
    while (1) {
        
        // The program relies on interrupts to handle button presses
        // No additional code is needed in the main loop
        _delay_ms(500);

    }

    return 0;
}
