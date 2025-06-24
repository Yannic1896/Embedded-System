#include "ses_scheduler.h"
#include "ses_led.h"
#include "ses_button.h"
#include <avr/io.h>
#include <stddef.h>
#include "ses_display.h"
#include <util/delay.h>
#include "ses_usbserial.h"


typedef enum
{
    RED,
    GREEN,
    YELLOW
} led_color;

// Set LED toggle color
static led_color toggle_color = GREEN;

void ledToggleTask(void *param);
void turnOffYellow(void *param);
void stopwatch(void *param);

bool yellow_led_on = false;
bool stopwatch_running = false;
volatile uint16_t stopwatch_time = 0;

task_descriptor_t blinkTask = {
    .task = &ledToggleTask,
    .param = &toggle_color,
    .expire = 2000, // 2s toggle
    .period = 2000
};

task_descriptor_t buttonDebounce = {
    .task = &button_checkState,
    .param = NULL,
    .expire = 5, // check button state every 5ms
    .period = 5
};

task_descriptor_t turnOffYellowTask = {
    .task = &turnOffYellow,
    .param = NULL,
    .expire = 5000, // 5s toggle
    .period = 0
};

task_descriptor_t startStopwatchTask = {
    .task = &stopwatch,
    .param = NULL,
    .expire = 100, // Update stopwatch every 100ms
    .period = 100
};

void ledToggleTask(void *param)
{
    led_color color = *((led_color *)param);

    switch (color)
    {
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
        // Do nothing
        break;
    }
}

void onPushButtonPressed(void)
{
    // Switch led status when push button is pressed
    if (!yellow_led_on)
    {
        led_yellowOn();
        yellow_led_on = true;
        scheduler_add(&turnOffYellowTask);
    }
    else
    {
        led_yellowOff();
        yellow_led_on = false;
        // Reset scheduler for next turn
        scheduler_remove(&turnOffYellowTask);
        turnOffYellowTask.expire = 5000;
    }
}

void onRotaryButtonPressed(void)
{
    // Reset the stopwatch when rotary button is pressed
    if (!stopwatch_running)
    {
        scheduler_add(&startStopwatchTask);
        stopwatch_running = true;
    }
    else
    {
        scheduler_remove(&startStopwatchTask);
        stopwatch_running = false;
        stopwatch_time = 0;
    }
}

void stopwatch(void *param)
{
    if (stopwatch_running)
    {
        stopwatch_time++;
        display_setCursor(0, 0);
        fprintf(displayout, "Time: %2u.%1u s", stopwatch_time / 10, stopwatch_time % 10);
        display_update();
    }
}

void turnOffYellow(void *param)
{
    led_yellowOff();
    yellow_led_on = false;
    scheduler_remove(&turnOffYellowTask);
    turnOffYellowTask.expire = 5000;
}

int main(void)
{
    // Initialize hardware
    led_redInit();
    led_yellowInit();
    led_greenInit();
    display_init();
    button_init(false);

    // set Callback functions
    button_setPushButtonCallback(onPushButtonPressed);
    button_setRotaryButtonCallback(onRotaryButtonPressed);

    // Initially turn LED's off
    led_redOff();
    led_yellowOff();
    led_greenOff();

    usbserial_init();

    // Initialize scheduler
    scheduler_init();

    scheduler_add(&blinkTask);
    scheduler_add(&buttonDebounce);

    scheduler_run();

    return 0;
}
