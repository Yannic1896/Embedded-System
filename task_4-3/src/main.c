#include "pscheduler.h"
#include <avr/io.h>
#include <util/delay.h>

// Simple task that blinks an LED on PORTB0
void taskLED1(void) {
    DDRB |= (1 << PB0);
    while(1) {
        PORTB ^= (1 << PB0);
        for(volatile int i = 0; i < 1000; i++); // Busy wait
    }
}

// Second task that blinks an LED on PORTB1 at different rate
void taskLED2(void) {
    DDRB |= (1 << PB1);
    while(1) {
        PORTB ^= (1 << PB1);
        for(volatile int i = 0; i < 1500; i++); // Busy wait
    }
}

// Third task that does some computation
void taskCompute(void) {
    volatile uint16_t counter = 0;
    while(1) {
        counter++;
        if(counter % 1000 == 0) {
            // Do something periodically
        }
    }
}

int main(void) {
    // Define our task array
    static const task_t TASKS[] = {taskLED1, taskLED2, taskCompute};
    
    // Calculate number of tasks
    #define NUM_TASKS (sizeof(TASKS)/sizeof(TASKS[0]))
    
    // Run the scheduler
    pscheduler_run(TASKS, NUM_TASKS);
    
    // This point should never be reached
    while(1);
    return 0;
}
