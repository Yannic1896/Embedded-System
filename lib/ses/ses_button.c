#include <avr/io.h>
#include <stdbool.h>
#include "ses_button.h"
#include <stddef.h>

// Button wiring on SES board
#define BUTTON_ROTARY_PIN   PINB
#define BUTTON_ROTARY_PORT  PORTB
#define BUTTON_ROTARY_DDR   DDRB
#define BUTTON_ROTARY_BIT   5

#define BUTTON_PUSH_PIN     PINB
#define BUTTON_PUSH_PORT    PORTB
#define BUTTON_PUSH_DDR     DDRB
#define BUTTON_PUSH_BIT     4

#define BUTTON_NUM_DEBOUNCE_CHECKS 5
#define BUTTON_DEBOUNCE_POS_PUSHBUTTON 0x01
#define BUTTON_DEBOUNCE_POS_ROTARYBUTTON 0x02

// Function pointers for button callbacks
pButtonCallback rotaryButtonCallback = NULL;
pButtonCallback pushButtonCallback = NULL;

void button_init(bool debouncing) {
    // Common initialization for both techniques
    
    // Configure rotary button pin as input
    BUTTON_ROTARY_DDR &= ~(1 << BUTTON_ROTARY_BIT);
    // Activate internal pull-up resistor for rotary button
    BUTTON_ROTARY_PORT |= (1 << BUTTON_ROTARY_BIT);

    // Configure push button pin as input
    BUTTON_PUSH_DDR &= ~(1 << BUTTON_PUSH_BIT);
    // Activate internal pull-up resistor for push button
    BUTTON_PUSH_PORT |= (1 << BUTTON_PUSH_BIT);

    if (debouncing) {
        // Initialization for debouncing technique
        timer1_setCallback(button_checkState);
        timer1_start();  // Start the debounce timer
        
        // Disable pin change interrupts for debouncing mode
        PCICR &= ~(1 << PCIE0);
        PCMSK0 &= ~((1 << BUTTON_ROTARY_BIT) | (1 << BUTTON_PUSH_BIT));
    } else {
        // Initialization for direct interrupt technique
        // Enable pin change interrupt for PORTB in PCICR
        PCICR |= (1 << PCIE0); // PCIE0 corresponds to PORTB
        
        // Enable pin change interrupt for the specific pins in PCMSK0
        PCMSK0 |= (1 << BUTTON_ROTARY_BIT) | (1 << BUTTON_PUSH_BIT);
        
        // Stop the debounce timer if it was running
        timer1_stop();
    }
}

bool button_isPushButtonPressed(void) {
    // Check if push button is pressed (logic low)
    return !(BUTTON_PUSH_PIN & (1 << BUTTON_PUSH_BIT));
}

bool button_isRotaryButtonPressed(void) {
    // Check if rotary button is pressed (logic low)
    return !(BUTTON_ROTARY_PIN & (1 << BUTTON_ROTARY_BIT));
}

void button_setRotaryButtonCallback(pButtonCallback callback) {
    rotaryButtonCallback = callback; // Store the callback function pointer
}

void button_setPushButtonCallback(pButtonCallback callback) {
    pushButtonCallback = callback; // Store the callback function pointer
}

void button_checkState()
{
    static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = { 0 };
    static uint8_t index = 0;
    static uint8_t debouncedState = 0;
    uint8_t lastDebouncedState = debouncedState;

    // each bit in every state byte represents one button
    state[index] = 0;
    if (button_isPushButtonPressed()) {
        state[index] |= BUTTON_DEBOUNCE_POS_PUSHBUTTON;
    }
    if (button_isRotaryButtonPressed()) {
        state[index] |= BUTTON_DEBOUNCE_POS_ROTARYBUTTON;
    }

    index++;
    if (index == BUTTON_NUM_DEBOUNCE_CHECKS) {
        index = 0;
    }

    // init compare value and compare with ALL reads, only if
    // we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1's" in the state
    // array, the button at this position is considered pressed
    uint8_t j = 0xFF;
    for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++) {
        j = j & state[i];
    }
    debouncedState = j;

    // Check for push button press (transition from not pressed to pressed)
    if ((debouncedState & BUTTON_DEBOUNCE_POS_PUSHBUTTON) && 
        !(lastDebouncedState & BUTTON_DEBOUNCE_POS_PUSHBUTTON) && 
        pushButtonCallback != NULL) {
        pushButtonCallback();
    }

    // Check for rotary button press (transition from not pressed to pressed)
    if ((debouncedState & BUTTON_DEBOUNCE_POS_ROTARYBUTTON) && 
        !(lastDebouncedState & BUTTON_DEBOUNCE_POS_ROTARYBUTTON) && 
        rotaryButtonCallback != NULL) {
        rotaryButtonCallback();
    }
}

ISR(PCINT0_vect) {
    // Check if the rotary button interrupt is unmasked and callback is set
    if ((PCMSK0 & (1 << BUTTON_ROTARY_BIT)) && rotaryButtonCallback != NULL) {
        if (button_isRotaryButtonPressed()&& pushButtonCallback != NULL) {
            rotaryButtonCallback(); // Execute rotary button callback
        }
    }

    // Check if the push button interrupt is unmasked and callback is set
    if ((PCMSK0 & (1 << BUTTON_PUSH_BIT)) && pushButtonCallback != NULL) {
        if (button_isPushButtonPressed()&& rotaryButtonCallback != NULL) {
            pushButtonCallback(); // Execute push button callback
        }
    }
}