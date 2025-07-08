#include "ses_fsm.h"
#include "ses_display.h"
#include "ses_scheduler.h"
#include "ses_led.h"
#include <avr/io.h>


void toggleRedLED(void* param);
void turnOffRedLED(void* param);
void refreshDisplay(void* param);

task_descriptor_t blinkLED = {
    .task = &toggleRedLED,
    .param = NULL,
    .expire = 0, // check button state every 5ms
    .period = 250
};

task_descriptor_t stopAlarm = {
    .task = &turnOffRedLED,
    .param = NULL,
    .expire = 5000, // check button state every 5ms
    .period = 0
};

task_descriptor_t updateDisplay = {
    .task = &refreshDisplay,
    .param = NULL,
    .expire = 0, // check button state every 5ms
    .period = 1000
};

// --- State Function Declarations ---
fsm_return_status_t state_uninitialized_setHour(fsm_t *fsm, const event_t *event) {
    switch (event->signal) {
        case ENTRY:
            fprintf(displayout,"00:00\nSet Time");
            display_update();
            fsm->timeSet.hour = 0;
            return RET_HANDLED;

        case ROTARYBUTTON_PRESSED:
            fsm->timeSet.hour = (fsm->timeSet.hour + 1) % 24;
            fprintf(displayout, "%02d:00\n", fsm->timeSet.hour);
            display_update();
            return RET_HANDLED;

        case PUSHBUTTON_PRESSED:
            fsm->state = state_uninitialized_setMinute;
            return RET_TRANSITION;

        case EXIT:
            return RET_HANDLED;
    }
    return RET_IGNORED;
}

fsm_return_status_t state_uninitialized_setMinute(fsm_t *fsm, const event_t *event) {
    switch (event->signal) {
        case ENTRY:
            fprintf(displayout, "%02d:00\n", fsm->timeSet.hour);
            display_update();
            fsm->timeSet.minute = 0;
            return RET_HANDLED;

        case ROTARYBUTTON_PRESSED:
            fsm->timeSet.minute = (fsm->timeSet.minute + 1) % 60;
            fprintf(displayout, "%02d:%02d\n", fsm->timeSet.hour, fsm->timeSet.minute);
            display_update();
            return RET_HANDLED;

        case PUSHBUTTON_PRESSED:
            fsm->state = state_normalOperation;
            system_time_t sysTime = timeToSystemTime(&fsm->timeSet);
            scheduler_setTime(sysTime);
            return RET_TRANSITION;

        case EXIT:
            return RET_HANDLED;
    }
    return RET_IGNORED;
}

fsm_return_status_t state_normalOperation(fsm_t *fsm, const event_t *event) {
    switch (event->signal) {
        case ENTRY:
            scheduler_add(&updateDisplay);
            return RET_HANDLED;

        case ROTARYBUTTON_PRESSED:
            fsm->isAlarmEnabled = !fsm->isAlarmEnabled;
            led_yellowToggle();
            return RET_HANDLED;

        case PUSHBUTTON_PRESSED:
            fsm->state = state_setAlarm_hour;
            scheduler_remove(&updateDisplay);
            return RET_TRANSITION;

        case TIME_MATCH:
            if (fsm->isAlarmEnabled) {
                fsm->state = state_alarmActive;
                return RET_TRANSITION;
            }
            return RET_IGNORED;
        case EXIT:
            return RET_HANDLED;
        default:
            return RET_IGNORED;
    }
}

fsm_return_status_t state_setAlarm_hour(fsm_t *fsm, const event_t *event) {
    switch (event->signal) {
        case ENTRY:
            fprintf(displayout,"00:00\nSet Time");
            fsm->timeSet.hour = 0;
            return RET_HANDLED;

        case ROTARYBUTTON_PRESSED:
            fsm->timeSet.hour = (fsm->timeSet.hour + 1) % 24;
            fprintf(displayout, "%02d:00\n", fsm->timeSet.hour);
            return RET_HANDLED;

        case PUSHBUTTON_PRESSED:
            fsm->state = state_setAlarm_minute;
            return RET_TRANSITION;

        case EXIT:
            return RET_HANDLED;
    }
    return RET_IGNORED;
}

fsm_return_status_t state_setAlarm_minute(fsm_t *fsm, const event_t *event) {
    switch (event->signal) {
        case ENTRY:
            fprintf(displayout, "%02d:00\n", fsm->timeSet.hour);
            fsm->timeSet.minute = 0;
            return RET_HANDLED;

        case ROTARYBUTTON_PRESSED:
            fsm->timeSet.minute = (fsm->timeSet.minute + 1) % 60;
            fprintf(displayout, "%02d:%02d\n", fsm->timeSet.hour, fsm->timeSet.minute);
            return RET_HANDLED;

        case PUSHBUTTON_PRESSED:
            fsm->state = state_normalOperation;
            return RET_TRANSITION;

        case EXIT:
            return RET_HANDLED;
    }
    return RET_IGNORED;
}


fsm_return_status_t state_alarmActive(fsm_t *fsm, const event_t *event) {
    switch (event->signal) {
        case ENTRY:
            scheduler_add(&blinkLED);
            scheduler_add(&stopAlarm);
            return RET_HANDLED;

        case PUSHBUTTON_PRESSED:
            fsm->state = state_normalOperation;
            return RET_TRANSITION;

        case ROTARYBUTTON_PRESSED:
            fsm->state = state_normalOperation;
            return RET_TRANSITION;
        case EXIT:
            return RET_HANDLED;
        default:
            return RET_IGNORED;
    }
}

void toggleRedLED(void* param){
    led_redToggle();
}

void turnOffRedLED(void* param){
    led_redOff();
    scheduler_remove(&blinkLED);
}

void refreshDisplay(void* param){
    system_time_t bufferTime = scheduler_getTime();
    time_t currentTime;
    systemTimeToTime(bufferTime, &currentTime);
    fprintf(displayout, "%02d:%02d:%02d\n",
        currentTime.hour,
        currentTime.minute,
        currentTime.second
    );
    display_update();
}

// --- FSM Dispatcher & Initialization ---
/* dispatches events to state machine, called in application*/
void fsm_dispatch(fsm_t * fsm, const event_t * event) {
    const event_t entryEvent = {.signal = ENTRY};
    const event_t exitEvent = {.signal = EXIT};

    state_t s = fsm->state;         // Save state in s
    fsm_return_status_t r = fsm->state(fsm, event);         // Execute state handler and save return in r

    /*Check if state transition ocurred*/
    if (r == RET_TRANSITION) {
        s(fsm, &exitEvent); //< call exit action of last state
        fsm->state(fsm, &entryEvent); //< call entry action of new state
    }
}

/* sets and calls initial state of state machine */
void fsm_init(fsm_t * fsm, state_t init) {
    //... other initialization
    const event_t entryEvent = {.signal = ENTRY};
    fsm->state = init;
    fsm->state(fsm, &entryEvent);
}