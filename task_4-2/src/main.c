#include "ses_scheduler.h"
#include "ses_led.h"
#include "ses_button.h"
#include <avr/io.h>
#include <stddef.h>
#include "ses_display.h"
#include <util/delay.h>

typedef enum{
    RED,
    GREEN,
    YELLOW
}led_color;

static led_color toggle_color = GREEN;

void led_toggle_task(void *param);
void turn_off_yellow(void * param);

bool yellow_led_on = false;

task_descriptor_t blinkTask = {
    .task = &led_toggle_task,   
    .param = &toggle_color,
    .expire = 2000,
    .period = 2000      
};

task_descriptor_t buttonDebounce = {
    .task = &button_checkState,   
    .param = NULL,
    .expire = 5,
    .period = 5 
};

task_descriptor_t turnOffYellowTask = {
    .task = &turn_off_yellow,
    .param = NULL,
    .expire = 5000,
    .period = 0
};

void led_toggle_task(void *param) {
    led_color color = *((led_color*)param);

    switch (color) {
        case RED:
            led_redToggle();
            break;
        case GREEN:
            led_greenToggle();
            break;
        case YELLOW:
            led_yellowToggle();
            break;
        default:
            // Do nothing or log error
            break;
    }
}

void onPushButtonPressed(void) {
    if (!yellow_led_on) {
        led_yellowOn();
        yellow_led_on = true;
        scheduler_add(&turnOffYellowTask);
    } else {
        led_yellowOff();
        yellow_led_on = false;
        scheduler_remove(&turnOffYellowTask);
    }
}

void turn_off_yellow(void *param) {
    led_yellowOff();
    yellow_led_on = false;
}

int main(void) {
    // Initialize hardware
    led_redInit();
    led_yellowInit();
    led_greenInit();
    display_init();
    button_init(true);
    
    button_setPushButtonCallback(onPushButtonPressed);

    // Initially turn LED's off
    led_redOff();
    led_yellowOff();
    led_greenOff();
    
    display_setCursor(0,0);
    fprintf(displayout, "Disp\n");
    display_update();

    usbserial_init();

    // Initialize scheduler
    scheduler_init();

    scheduler_add(&blinkTask);
    
    scheduler_add(&buttonDebounce);

    scheduler_run();

    return 0;
}

