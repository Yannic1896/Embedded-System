/* INCLUDES ******************************************************************/
#include "ses_timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* DEFINES & MACROS **********************************************************/
#define TIMER0_CYC_FOR_1MILLISEC    ((F_CPU / 64 / 1000) - 1)
#define TIMER1_CYC_FOR_5MILLISEC    ((F_CPU / 64 / 200) - 1)

/* GLOBAL VARIABLES **********************************************************/
static pTimerCallback timer0_callback = 0;
static pTimerCallback timer1_callback = 0;

/* FUNCTION DEFINITIONS ******************************************************/
void timer0_setCallback(pTimerCallback cb) {
    timer0_callback = cb;
}

void timer0_start() {
    TCCR0A = (1 << WGM01);                        // CTC mode
    TCCR0B = (1 << CS01) | (1 << CS00);           // Prescaler = 64
    OCR0A = TIMER0_CYC_FOR_1MILLISEC;             // Set compare value for 1ms
    TIMSK0 |= (1 << OCIE0A);                      // Enable Timer0 Compare A interrupt
    TIFR0 |= (1 << OCF0A);                        // Clear interrupt flag
}

void timer0_stop() {
    TCCR0B = 0;                                   // Stop Timer0
    TIMSK0 &= ~(1 << OCIE0A);                     // Disable Timer0 interrupt
}

void timer1_setCallback(pTimerCallback cb) {
    timer1_callback = cb;
}

void timer1_start() {
    TCCR1B |= (1 << WGM12);                       // CTC mode
    TCCR1B |= (1 << CS11) | (1 << CS10);          // Prescaler = 64
    OCR1A = TIMER1_CYC_FOR_5MILLISEC;             // Set compare value for 5ms
    TIMSK1 |= (1 << OCIE1A);                      // Enable Timer1 Compare A interrupt
    TIFR1 |= (1 << OCF1A);                        // Clear interrupt flag
}

void timer1_stop() {
    TCCR1B = 0;                                   // Stop Timer1
    TIMSK1 &= ~(1 << OCIE1A);                     // Disable Timer1 interrupt
}

/* INTERRUPT SERVICE ROUTINES ************************************************/
ISR(TIMER0_COMPA_vect) {
    if (timer0_callback != 0) {
        timer0_callback();
    }
}

/*ISR(TIMER1_COMPA_vect) {
    if (timer1_callback != 0) {
        timer1_callback();
    }
}*/

