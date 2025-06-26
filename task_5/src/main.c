#include <avr/io.h>
#include <avr/interrupt.h>
#include "ses_led.h"
#include "ses_button.h"
#include "ses_fan.h"
#include "ses_adc.h"
#include "ses_scheduler.h"
#include "ses_display.h"
#include <stddef.h>
#include "ses_usbserial.h"
#include "ses_fanspeed.h"

#define POT_ADC_CHANNEL 

// define module-private variables
static uint16_t fanspeed = 0;           
static uint16_t filteredFanspeed = 0;           
static uint8_t dutycycle = 0;           


void toggleFanTask(void *param);
void potTask(void* param);
void displayCurrentRPM(void* param);

bool fan_running = false;

task_descriptor_t PotTask = {
    .task = &potTask,
    .param = NULL,
    .expire = 50, // Read potentiometer every 50ms
    .period = 50
};

task_descriptor_t buttonDebounce = {
    .task = &button_checkState,
    .param = NULL,
    .expire = 5, // check button state every 5ms
    .period = 5
};

task_descriptor_t displayFanspeed = {
    .task = &displayCurrentRPM,
    .param = NULL,
    .expire = 15, 
    .period = 1000   // update display every 1s
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

void potTask(void *param) {
    if (fan_running) {
        uint16_t potValue = adc_read(ADC_POTI_CH);
        uint8_t duty = potValue >> 2; // Scale 10-bit ADC to 8-bit PWM (0..255)
        fan_setDutyCycle(duty);
        uint16_t rpm = 0;
        rpm = fanspeed_getRecent();
        //fprintf(serialout, "RPM: %u\nDuty:  %u\n", rpm, duty);
        dutycycle = duty;
    } else {
        fan_setDutyCycle(0);
        dutycycle = 0;
    }
}

void displayCurrentRPM(void *param) {
    fanspeed = fanspeed_getRecent();
    filteredFanspeed = fanspeed_getFiltered();
    display_setCursor(0, 0);
    fprintf(displayout,"RPM:  %u\nFiltered:  %u\nDuty:  %u\n", fanspeed, filteredFanspeed, dutycycle);
    display_update();
}

int main(void) {
    // Init hardware
    led_redInit();
    led_yellowInit();
    led_redOff();
    button_init(false); // false: use interrupt/scheduler, not timer debounce
    fan_init();
    adc_init();
    scheduler_init();

    usbserial_init();
    
    fanspeed_init();

    button_setPushButtonCallback(toggleFan);

    // Schedule tasks: button check every 10ms, pot read every 50ms
    scheduler_add(&buttonDebounce);
    scheduler_add(&PotTask);
    scheduler_add(&displayFanspeed);

    sei(); // Enable global interrupts

    // RPM min: 451
    // RPM max: 2176
    scheduler_run();
    

}