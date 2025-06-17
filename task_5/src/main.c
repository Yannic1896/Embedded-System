#include <avr/io.h>
#include <avr/interrupt.h>
#include "ses_led.h"
#include "ses_button.h"
#include "ses_fan.h"
#include "ses_adc.h"
#include "ses_scheduler.h"

#define POT_ADC_CHANNEL 

volatile uint8_t fanOn = 0;

void toggleFanTask(void) {
    static uint8_t lastButtonState = 0;
    uint8_t currentState = button_isPushButtonPressed();

    // Debounce: only toggle on rising edge
    if (currentState && !lastButtonState) {
        if (fanOn) {
            fan_disable();
            fanOn = 0;
            led_redOff();
        } else {
            fan_enable();
            fanOn = 1;
            led_redOn();
        }
    }
    lastButtonState = currentState;
}

void potTask(void) {
    if (fanOn) {
        uint16_t potValue = adc_read(POT_ADC_CHANNEL); // 0..1023
        uint8_t duty = potValue >> 2; // Scale 10-bit ADC to 8-bit PWM (0..255)
        fan_setDutyCycle(duty);
    } else {
        fan_setDutyCycle(0);
    }
}

int main(void) {
    // Init hardware
    led_redInit();
    led_redOff();
    button_init(false); // false: use interrupt/scheduler, not timer debounce
    fan_init();
    adc_init();
    scheduler_init();

    // Schedule tasks: button check every 10ms, pot read every 50ms
    scheduler_setTask(toggleFanTask, 10);
    scheduler_setTask(potTask, 50);

    sei(); // Enable global interrupts

    while (1) {
        scheduler_run();
    }
}