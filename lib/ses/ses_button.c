/* INCLUDES ******************************************************************/

#include <avr/io.h>
#include "ses_led.h"
#include "ses_button.h"
#include <stddef.h>

/* DEFINES & MACROS **********************************************************/

// LED wiring on SES board
#define BUTTON_ROTARY_PORT          PORTB
#define BUTTON_ROTARY_DDR           DDRB
#define BUTTON_ROTARY_BIT           5

#define BUTTON_PUSH_PORT            PORTB
#define BUTTON_PUSH_DDR           DDRB
#define BUTTON_PUSH_BIT             4

static pButtonCallback rotaryButtonCallback = NULL;
static pButtonCallback pushButtonCallback = NULL;

static volatile uint8_t lastButtonState = 0xFF;  // Initial state: buttons not pressed (pull-up = high)


/* FUNCTION DEFINITION ******************************************************/

void button_init(void){
    DDRB &= ~((1 << BUTTON_ROTARY_BIT) | (1 << BUTTON_PUSH_BIT)); // Define Bits as input
    PORTB |= (1 << BUTTON_ROTARY_BIT) | (1 << BUTTON_PUSH_BIT);     // Enable internal pullup resistors

    PCICR |= (1 << PCIE0);      // Activate pin change interrupt for a specific Port (PortB) 
    PCMSK0 |= (1 << BUTTON_PUSH_BIT) | (1 << BUTTON_ROTARY_BIT);        // Enable triggering an interrupt when Button is pressed
    // PCMSK0 controls which specific pins on the port can trigger a pin change interrupt
}

 /* Get the state of the pushbutton.
 */
_Bool button_isPushButtonPressed(void){
    return !(PINB & (1 << BUTTON_PUSH_BIT));        // Return true if button pushed
}

/** 
 * Get the state of the rotary button.
 */
_Bool button_isRotaryButtonPressed(void){
    return !(PINB & (1 << BUTTON_ROTARY_BIT));      // Return true if button pushed
}

ISR(PCINT0_vect)
{// execute callbacks here
    uint8_t currentState = PINB;
    uint8_t changed = currentState ^ lastButtonState;  // XOR to find changed bits
    lastButtonState = currentState;

    // Check Push Button
    if ((changed & (1 << BUTTON_PUSH_BIT)) && (PCMSK0 & (1 << BUTTON_PUSH_BIT))) {      // Check if Push button has changed and corresponding interrupt is unmasked
        if (pushButtonCallback != NULL) {     // Check if callback is set
            pushButtonCallback();
        }
    }

    // Check Rotary Button
    if ((changed & (1 << BUTTON_ROTARY_BIT)) && (PCMSK0 & (1 << BUTTON_ROTARY_BIT))) {
        if (rotaryButtonCallback != NULL) {
            rotaryButtonCallback();
        }
    }
}

// Callback setter functions
void button_setRotaryButtonCallback(pButtonCallback callback) {
    rotaryButtonCallback = callback;
}

void button_setPushButtonCallback(pButtonCallback callback) {
    pushButtonCallback = callback;
}