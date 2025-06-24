#include <avr/io.h>
#include "ses_fan.h"
#include "ses_fanspeed.h"
#include <avr/interrupt.h>
#include "ses_led.h"
#include <stdbool.h>

// Module-private variables
static volatile uint16_t edgeCount = 0;
static volatile bool newMeasurement = false;
static volatile uint16_t currentRpm = 0;

void fanspeed_init(void) {
    // Configure INT6 (PE6) for rising edge detection
    EICRB |= (1 << ISC61) | (1 << ISC60); // Rising edge triggers interrupt
    EIMSK |= (1 << INT6); // Enable INT6

    // Configure Timer1 for 1 second measurement intervals in CTC mode
    TCCR1A = 0; // Normal port operation, CTC mode
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, prescaler 1024

    // For 16MHz clock: 16,000,000 / 1024 = 15625 counts per second
    OCR1A = 15624; // Compare match every 1s (count from 0 to 15624)

    TIMSK1 = (1 << OCIE1A); // Enable Output Compare A Match interrupt

}

ISR(INT6_vect) {
    // Interrupt service routine for tacho signal edges
    edgeCount++;
    led_yellowToggle(); // Visual feedback
}

ISR(TIMER1_COMPA_vect) {
    // Timer3 compare match interrupt - measurement interval complete
    cli(); // Disable interrupts for atomic access
    uint16_t edges = edgeCount;
    edgeCount = 0;
    sei(); // Re-enable interrupts
    
    if (edges == 0) {
        // Fan has stopped
        currentRpm = 0;
        led_redOn(); // Turn on red LED when fan stops
    } else {
        // Calculate RPM: edges per revolution = 2, measurement time = 1s
        currentRpm = edges * 30;
        led_redOff(); // Turn off red LED when fan is running
    }
    
    newMeasurement = true;
}

uint16_t fanspeed_getRecent(void) {
    uint16_t rpm;
    
    cli(); // Disable interrupts for atomic access
    rpm = currentRpm;
    newMeasurement = false;
    sei(); // Re-enable interrupts
    
    return rpm;
}
