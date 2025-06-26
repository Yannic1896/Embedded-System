#include <avr/io.h>
#include <avr/interrupt.h>
#include "ses_led.h"
#include "ses_button.h"
#include "ses_fan.h"
#include "ses_adc.h"
#include "ses_scheduler.h"
#include <stddef.h>
#include "ses_usbserial.h"
#include "ses_fanspeed.h"
#include "ses_display.h"

void toggleFan(void *param);
void potTask(void* param);
void displayTask(void *param);

bool fan_running = false;
uint8_t lastDuty = 0; // Store last duty cycle for display

task_descriptor_t PotTask = {
    .task = &potTask,
    .param = NULL,
    .expire = 50, 
    .period = 50 // Read potentiometer every 50ms
};

task_descriptor_t buttonDebounce = {
    .task = &button_checkState,
    .param = NULL,
    .expire = 5, 
    .period = 5 // check button state every 5ms
};

task_descriptor_t DisplayTask = {
    .task = &displayTask,
    .param = NULL,
    .expire = 1000,
    .period = 1000  // Repeat every 1s
};

void toggleFan(void *param) {
    if (!fan_running){
        fan_enable();
        fan_running = true;
        led_redOn();
    }else{
        fan_disable();
        fan_running = false;
        led_redOff();
    }
}
// Task to read the potentiometer and set fan duty cycle
void potTask(void *param) {
    if (fan_running) {
        uint16_t potValue = adc_read(ADC_POTI_CH);
        uint8_t duty = potValue >> 2; // Scale 10-bit ADC to 8-bit PWM (0..255)
        fan_setDutyCycle(duty);
        lastDuty = duty; // Store for display
    } else {
        fan_setDutyCycle(0);
        lastDuty = 0;
    }
}
//Display RPM, filtered RPM, and duty cycle on the display and serial output
void displayTask(void *param) {
    uint16_t rpm = fanspeed_getRecent();
    uint16_t filteredRpm = fanspeed_getFiltered();
    display_clear(); 
    display_setCursor(0, 0); // Set cursor to top-left
    fprintf(displayout, "RPM: %u\nFiltered: %u\nDuty: %u\n", rpm, filteredRpm, lastDuty);
    fprintf(serialout, "RPM: %u\nFiltered: %u\nDuty: %u\n", rpm, filteredRpm, lastDuty);

    display_update(); 
}

int main(void) {
    // Init hardware
    led_redInit();
    led_redOff();
    button_init(false); // false: use interrupt/scheduler, not timer debounce
    fan_init();
    adc_init();
    scheduler_init();
    display_init();
    usbserial_init();
    fanspeed_init();

    button_setPushButtonCallback(toggleFan);

    // Schedule tasks
    scheduler_add(&buttonDebounce);
    scheduler_add(&PotTask);
    scheduler_add(&DisplayTask); 

    sei(); // Enable global interrupts

    scheduler_run();
}