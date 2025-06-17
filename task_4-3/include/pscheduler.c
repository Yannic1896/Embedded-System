#include "pscheduler.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

// Task Control Block structure
typedef struct {
    uint8_t *stack;     // Pointer to the task's stack
    uint8_t *sp;        // Saved stack pointer
    uint8_t in_use;     // Flag indicating if this TCB is in use
} tcb_t;

// Scheduler variables
static tcb_t *current_task = 0;
static tcb_t *next_task = 0;
static tcb_t task_list[8];  // Support up to 8 tasks
static uint8_t num_tasks = 0;
static uint8_t current_task_index = 0;

// Timer interrupt for 1ms time slices (assuming 16MHz clock)
void setup_timer() {
    // Configure Timer1 for 1ms interrupts
    TCCR1A = 0;                     // Normal mode
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS11);  // CTC mode, prescaler 64
    OCR1A = 249;                    // Compare value for 1ms
    TIMSK1 = (1 << OCIE1A);        // Enable compare interrupt
}

// Initialize a task's stack to look like it has just been called
void init_task_stack(tcb_t *tcb, task_t task) {
    // Allocate stack space (128 bytes per task)
    tcb->stack = (uint8_t*)malloc(128);
    uint8_t *sp = tcb->stack + 128 - 1;
    
    // Initialize stack to look like the task was interrupted
    // Order of registers pushed by hardware during interrupt: PC, SREG, R0-R31
    *sp-- = (uint8_t)((uint16_t)task >> 8);    // PC high
    *sp-- = (uint8_t)((uint16_t)task);         // PC low
    *sp-- = 0x80;                              // SREG (I flag set)
    
    // Initialize general purpose registers to 0
    for (uint8_t i = 0; i < 32; i++) {
        *sp-- = 0;
    }
    
    tcb->sp = sp;
    tcb->in_use = 1;
}

// Context switch ISR
ISR(TIMER1_COMPA_vect) {
    // Save current task context
    uint8_t sp_l, sp_h;
    
    asm volatile(
        "push r0              \n\t"  // Save R0
        "in r0, __SREG__      \n\t"  // Save SREG
        "cli                  \n\t"  // Disable interrupts
        "push r0              \n\t"
        
        // Save remaining registers R1-R31
        "push r1              \n\t"
        "push r2              \n\t"
        "push r3              \n\t"
        "push r4              \n\t"
        "push r5              \n\t"
        "push r6              \n\t"
        "push r7              \n\t"
        "push r8              \n\t"
        "push r9              \n\t"
        "push r10             \n\t"
        "push r11             \n\t"
        "push r12             \n\t"
        "push r13             \n\t"
        "push r14             \n\t"
        "push r15             \n\t"
        "push r16             \n\t"
        "push r17             \n\t"
        "push r18             \n\t"
        "push r19             \n\t"
        "push r20             \n\t"
        "push r21             \n\t"
        "push r22             \n\t"
        "push r23             \n\t"
        "push r24             \n\t"
        "push r25             \n\t"
        "push r26             \n\t"
        "push r27             \n\t"
        "push r28             \n\t"
        "push r29             \n\t"
        "push r30             \n\t"
        "push r31             \n\t"
        
        // Save stack pointer
        "in %0, __SP_L__      \n\t"
        "in %1, __SP_H__      \n\t"
        : "=r" (sp_l), "=r" (sp_h)
        :
        : "r0"
    );
    
    current_task->sp = (uint8_t*)((sp_h << 8) | sp_l);
    
    // Round-robin scheduling
    current_task_index = (current_task_index + 1) % num_tasks;
    next_task = &task_list[current_task_index];
    
    // Restore next task's context
    sp_l = (uint8_t)((uint16_t)next_task->sp);
    sp_h = (uint8_t)((uint16_t)next_task->sp >> 8);
    
    asm volatile(
        // Restore stack pointer
        "out __SP_L__, %0     \n\t"
        "out __SP_H__, %1      \n\t"
        
        // Restore registers R31-R1
        "pop r31              \n\t"
        "pop r30              \n\t"
        "pop r29              \n\t"
        "pop r28              \n\t"
        "pop r27              \n\t"
        "pop r26              \n\t"
        "pop r25              \n\t"
        "pop r24              \n\t"
        "pop r23              \n\t"
        "pop r22              \n\t"
        "pop r21              \n\t"
        "pop r20              \n\t"
        "pop r19              \n\t"
        "pop r18              \n\t"
        "pop r17              \n\t"
        "pop r16              \n\t"
        "pop r15              \n\t"
        "pop r14              \n\t"
        "pop r13              \n\t"
        "pop r12              \n\t"
        "pop r11              \n\t"
        "pop r10              \n\t"
        "pop r9               \n\t"
        "pop r8               \n\t"
        "pop r7               \n\t"
        "pop r6               \n\t"
        "pop r5               \n\t"
        "pop r4               \n\t"
        "pop r3               \n\t"
        "pop r2               \n\t"
        "pop r1               \n\t"
        
        // Restore SREG and R0
        "pop r0               \n\t"
        "out __SREG__, r0     \n\t"
        "pop r0               \n\t"
        
        // Return from interrupt (will restore PC)
        "reti                 \n\t"
        : 
        : "r" (sp_l), "r" (sp_h)
        : "r0"
    );
}

// Initialize and run the scheduler
void pscheduler_run(const task_t *taskArray, uint8_t len) {
    // Initialize tasks
    num_tasks = len;
    for (uint8_t i = 0; i < num_tasks; i++) {
        init_task_stack(&task_list[i], taskArray[i]);
    }
    
    // Set first task
    current_task = &task_list[0];
    current_task_index = 0;
    
    // Set up timer interrupt
    setup_timer();
    sei();  // Enable global interrupts
    
    // Load first task context
    uint8_t sp_l = (uint8_t)((uint16_t)current_task->sp);
    uint8_t sp_h = (uint8_t)((uint16_t)current_task->sp >> 8);
    
    asm volatile(
        // Set stack pointer to task's stack
        "out __SP_L__, %0     \n\t"
        "out __SP_H__, %1      \n\t"
        
        // Pop registers to initialize context
        "pop r31              \n\t"
        "pop r30              \n\t"
        "pop r29              \n\t"
        "pop r28              \n\t"
        "pop r27              \n\t"
        "pop r26              \n\t"
        "pop r25              \n\t"
        "pop r24              \n\t"
        "pop r23              \n\t"
        "pop r22              \n\t"
        "pop r21              \n\t"
        "pop r20              \n\t"
        "pop r19              \n\t"
        "pop r18              \n\t"
        "pop r17              \n\t"
        "pop r16              \n\t"
        "pop r15              \n\t"
        "pop r14              \n\t"
        "pop r13              \n\t"
        "pop r12              \n\t"
        "pop r11              \n\t"
        "pop r10              \n\t"
        "pop r9               \n\t"
        "pop r8               \n\t"
        "pop r7               \n\t"
        "pop r6               \n\t"
        "pop r5               \n\t"
        "pop r4               \n\t"
        "pop r3               \n\t"
        "pop r2               \n\t"
        "pop r1               \n\t"
        "pop r0               \n\t"
        "out __SREG__, r0     \n\t"
        "pop r0               \n\t"
        
        // Jump to task
        "ret                  \n\t"
        : 
        : "r" (sp_l), "r" (sp_h)
        : "r0"
    );
    
    // Should never reach here
    while(1);
}