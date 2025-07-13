#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "ses_fsm.h"
#include "ses_scheduler.h"
#include "ses_button.h"
#include "ses_led.h"
#include "ses_display.h"
#include "ses_usbserial.h"
#include <stddef.h>

static fsm_t alarmFSM;
// #define _DEBUG_SERIAL_

task_descriptor_t buttonDebounce = {
    .task = &button_checkState,
    .param = NULL,
    .expire = 5,
    .period = 5 // check button state every 5ms
};

typedef struct
{
    fsm_t *fsm;
    event_t event;
} fsm_dispatch_params_t;

void fsm_dispatchTaskFunction(void *param)
{
    fsm_dispatch_params_t *args = (fsm_dispatch_params_t *)param;
    fsm_dispatch(args->fsm, &args->event);
}

void pushButtonPressed()
{
    static task_descriptor_t pushTask;
    static fsm_dispatch_params_t pushParams;

    pushParams.fsm = &alarmFSM;
    pushParams.event.signal = PUSHBUTTON_PRESSED;

    pushTask.task = &fsm_dispatchTaskFunction;
    pushTask.param = &pushParams;
    pushTask.expire = 1;
    pushTask.period = 0;

    scheduler_add(&pushTask);
}

void rotaryButtonPressed()
{
    static task_descriptor_t rotaryTask;
    static fsm_dispatch_params_t rotaryParams;

    rotaryParams.fsm = &alarmFSM;
    rotaryParams.event.signal = ROTARYBUTTON_PRESSED;

    rotaryTask.task = &fsm_dispatchTaskFunction;
    rotaryTask.param = &rotaryParams;
    rotaryTask.expire = 1;
    rotaryTask.period = 0;

    scheduler_add(&rotaryTask);
}

int main(void)
{
    sei();
    button_init(false); // false: use interrupt/scheduler, not timer debounce
    usbserial_init();
    display_init();
    display_clear();
    scheduler_init();
    led_redInit();
    led_greenInit();
    led_yellowInit();
    led_redOff();
    led_yellowOff();
    fsm_init(&alarmFSM, state_uninitialized_setHour);

    button_setPushButtonCallback(pushButtonPressed);
    button_setRotaryButtonCallback(rotaryButtonPressed);

    scheduler_add(&buttonDebounce);

    scheduler_run();
}
