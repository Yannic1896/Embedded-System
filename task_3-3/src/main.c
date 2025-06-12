<<<<<<< HEAD
#include "ses_button.h"
#include "ses_led.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


int main(void) {
    // Initialize LEDs
    led_redInit();
    led_greenInit();
    sei();

    // Initialize buttons
    button_init(true);

    // Set button callbacks
    button_setRotaryButtonCallback(led_redToggle);   // Rotary button toggles red LED
    button_setPushButtonCallback(led_greenToggle);  // Push button toggles green LED

    // Main loop
    while (1) {
        
        // The program relies on interrupts to handle button presses
        // No additional code is needed in the main loop
        _delay_ms(500);

    }

    return 0;
}

=======

int main(void) {

}
>>>>>>> Jannis
