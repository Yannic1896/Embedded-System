#include <avr/io.h>
#include <avr/interrupt.h>
#include "ses_timer.h"
#include "ses_led.h"


volatile uint16_t softwareCounter = 0;

/* This function is called every 1ms by Timer0 ISR */
void softwareTimer(void) {
    softwareCounter++;
    if (softwareCounter >= 1000) {
        led_yellowToggle();  // Toggle LED every 1000 ms (1 second)
        softwareCounter = 0;
    }
}

int main(void) {
    // Initialize yellow LED pin as output
    led_yellowInit();

    // Set softwareTimer as Timer0 callback
    timer0_setCallback(softwareTimer);

    // Start Timer0 (1 ms interrupt)
    timer0_start();

    // Enable global interrupts
    sei();

    while (1) {
        // Main loop does nothing; everything is handled by interrupt
    }
}
