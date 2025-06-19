#include <avr/io.h>
#include <avr/interrupt.h>
#include "ses_led.h"
#include "ses_button.h"
#include "ses_fan.h"
#include "ses_adc.h"
#include "ses_scheduler.h"
#include <stddef.h>
#include <stdio.h>
#include "ses_usbserial.h"
#include <util/delay.h>

// Function prototypes
void toggleFanTask(void *param);
void potTask(void* param);

bool fan_running = false;

task_descriptor_t PotTask = {
    .task = &potTask,
    .param = NULL,
    .expire = 50,
    .period = 100
};

task_descriptor_t buttonDebounce = {
    .task = &button_checkState,
    .param = NULL,
    .expire = 5,
    .period = 5
};

void toggleFan(void *param) {
    if (!fan_running){
        fan_enable();
        fan_running = true;
        led_redOn();
    } else {
        fan_disable();
        fan_running = false;
        led_redOff();
    }
}

void potTask(void *param) {
    if (fan_running) {
        uint16_t potValue = adc_read(ADC_POTI_CH);
        uint8_t duty = potValue >> 2; //Shift to scale to duty cycle (0-255)
        fan_setDutyCycle(duty);
    } else {
        fan_setDutyCycle(0);
    }
}

int main(void) {
    // Init hardware
    led_redInit();
    led_redOff();
    button_init(true);
    fan_init();
    adc_init();
    scheduler_init();
    usbserial_init();

    //Set callback for button press
    button_setPushButtonCallback(toggleFan);

    // Schedule tasks
    scheduler_add(&buttonDebounce);
    scheduler_add(&PotTask);

    sei(); // Enable global interrupts

    scheduler_run();
}