#include <avr/io.h>
#include <stdbool.h>
#include "ses_button.h"

// Button wiring on SES board
#define BUTTON_ROTARY_PIN   PINB
#define BUTTON_ROTARY_PORT  PORTB
#define BUTTON_ROTARY_DDR   DDRB
#define BUTTON_ROTARY_BIT   6

#define BUTTON_PUSH_PIN     PINB
#define BUTTON_PUSH_PORT    PORTB
#define BUTTON_PUSH_DDR     DDRB
#define BUTTON_PUSH_BIT     7

void button_init(void) {
    // Configure rotary button pin as input
    BUTTON_ROTARY_DDR &= ~(1 << BUTTON_ROTARY_BIT);
    // Activate internal pull-up resistor for rotary button
    BUTTON_ROTARY_PORT |= (1 << BUTTON_ROTARY_BIT);

    // Configure push button pin as input
    BUTTON_PUSH_DDR &= ~(1 << BUTTON_PUSH_BIT);
    // Activate internal pull-up resistor for push button
    BUTTON_PUSH_PORT |= (1 << BUTTON_PUSH_BIT);
}

bool button_isPushButtonPressed(void) {
    // Check if push button is pressed (logic low)
    return !(BUTTON_PUSH_PIN & (1 << BUTTON_PUSH_BIT));
}

bool button_isRotaryButtonPressed(void) {
    // Check if rotary button is pressed (logic low)
    return !(BUTTON_ROTARY_PIN & (1 << BUTTON_ROTARY_BIT));
}

