#include "ses_scheduler.h"
#include "ses_led.h"
#include "ses_button.h"
#include <avr/io.h>
#include <stddef.h>
#include "ses_display.h"

// Task function prototypes
void blink_red(void* param);
void blink_yellow(void* param);
void stop_yellow_blink(void* param);

// Task descriptors
task_descriptor_t blinkRedTask = {
    .task = &blink_red,
    .param = NULL,
    .expire = 1000,  // Start after 1 second
    .period = 0      // One-shot task
};

task_descriptor_t blinkYellowTask = {
    .task = &blink_yellow,
    .param = NULL,
    .expire = 500,   // Start after 0.5 seconds
    .period = 500    // Repeat every 0.5 seconds
};

task_descriptor_t stopYellowTask = {
    .task = &stop_yellow_blink,
    .param = NULL,
    .expire = 10000,  // Run after 5 seconds
    .period = 0      // One-shot task
};

int main(void) {
    // Initialize hardware
    led_redInit();
    led_yellowInit();
    display_init();
    
    // Initially turn LED's off
    led_redOff();
    led_yellowOff();
    
    display_setCursor(0,0);
    fprintf(displayout, "Disp\n");
    display_update();
    // Initialize scheduler
    scheduler_init();
    
    // Add tasks to scheduler
    scheduler_add(&blinkRedTask);
    scheduler_add(&blinkYellowTask);
    scheduler_add(&stopYellowTask);
    
    // Run scheduler (this never returns)
    scheduler_run();
    
    return 0;
}

void blink_red(void* param) {
    (void)param; // Unused parameter
    led_redToggle();
    // This is a one-shot task, so it will be automatically removed
}

void blink_yellow(void* param) {
    (void)param; // Unused parameter
    led_yellowToggle();
    // This is a periodic task, so it will keep running
}

void stop_yellow_blink(void* param) {
    (void)param; // Unused parameter
    scheduler_remove(&blinkYellowTask);
    led_yellowOff();
}