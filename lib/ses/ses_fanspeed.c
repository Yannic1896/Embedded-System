#include <avr/io.h>
#include "ses_fan.h"
#include "ses_timer.h"
#include "ses_fanspeed.h"
#include <avr/interrupt.h>
#include "ses_led.h"
#include <stdbool.h>

#define FILTER_SIZE 5       // Size of median filter

// Module-private variables
static volatile uint16_t edgeCount = 0;
static volatile bool newMeasurement = false;
static volatile uint16_t currentRpm = 0;


// Add buffer to store recent RPMs
static uint16_t rpmBuffer[FILTER_SIZE];
static uint8_t rpmIndex = 0;   // Points to the next position to write

void fanspeed_init(void) {
    // Configure INT6 (PE6) for rising edge detection
    EICRB |= (1 << ISC61) | (1 << ISC60); // Rising edge triggers interrupt
    EIMSK |= (1 << INT6); // Enable INT6

    timer1_setCallback(update_rpm)
    timer1_start();

}

ISR(INT6_vect) {
    // Interrupt service routine for tacho signal edges
    edgeCount++;
    led_yellowToggle(); // Visual feedback
}

void update_rpm(void) {
    cli(); // Atomic access to edgeCount
    uint16_t edges = edgeCount;
    edgeCount = 0;
    sei(); // Allow interrupts again

    uint16_t rpm;
    if (edges == 0) {
        rpm = 0;
        led_redOn();
    } else {
        rpm = edges * 30; // Calculate RPM
        led_redOff();
    }

    cli(); // Short atomic block to update shared variables
    currentRpm = rpm;
    rpmBuffer[rpmIndex] = rpm;
    rpmIndex = (rpmIndex + 1) % FILTER_SIZE;  // Circular buffer
    sei();

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


uint16_t fanspeed_getFiltered(void) {
    uint16_t copy[FILTER_SIZE];

    // Copy buffer safely (short atomic block)
    cli();
    for (uint8_t i = 0; i < FILTER_SIZE; i++) {
        copy[i] = rpmBuffer[i];
    }
    sei();

    // Manual insertion sort on the copy
    for (uint8_t i = 1; i < FILTER_SIZE; i++) {
        uint16_t key = copy[i];
        int8_t j = i - 1;
        while (j >= 0 && copy[j] > key) {
            copy[j + 1] = copy[j];
            j--;
        }
        copy[j + 1] = key;
    }

    // Return median
    return copy[FILTER_SIZE / 2]; // Middle element
}
