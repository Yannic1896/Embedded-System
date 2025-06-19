#include <avr/io.h>
#include "ses_fan.h"

#define FAN_DDR   DDRD
#define CONTROL_DDR DDRC
#define FAN_PORT  PORTD
#define FAN_PIN   PD4 
#define FAN_CONTROL_PIN PC6

void fan_init(void) {
    // Set PD2 (OC3A) as output
    FAN_DDR |= (1 << FAN_PIN);
    CONTROL_DDR |= (1 << FAN_CONTROL_PIN); // Set PC6 as output to indicate fan connection

    // Enable Timer3 by clearing PRTIM3 in PRR1
    PRR1 &= ~(1 << PRTIM3);

    // Fast PWM, 8-bit: WGM3[3:0] = 0b0101 (see datasheet Table 14-7)
    TCCR3A = (1 << WGM30);
    TCCR3B = (1 << WGM32);

    // Compare Output Mode: Clear OC3A on compare match, set at BOTTOM (non-inverting)
    TCCR3A |= (1 << COM3A1);

    // Prescaler: clk/64 (for ~1ms period at 16MHz: 16e6/64/256 ≈ 976Hz)
    TCCR3B |= (1 << CS31) | (1 << CS30);

    // Initialize duty cycle to 0 (fan off)
    OCR3A = 0;
}

void fan_setDutyCycle(uint8_t dc) {
    OCR3A = dc;
}

void fan_enable(void) {
    FAN_PORT |= (1 << FAN_PIN); // Set PD4 high to start PWM output
    // Power on: set output high (if needed, or just ensure PWM is running)
    // If fan power is controlled by another pin, set it here.
    // For pure PWM, nothing extra is needed.
}

void fan_disable(void) {
    FAN_PORT &= ~(1 << FAN_PIN); // Set PD2 low to stop PWM output
    // Set duty cycle to 0 (fan off)
    OCR3A = 0;
    // Optionally, power off the fan supply if controlled by another pin.
}