#ifndef FSM_H_
#define FSM_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include "ses_scheduler.h"

typedef struct fsm_s fsm_t;     //< typedef for alarm clock state machine
typedef struct event_s event_t; //< event type for alarm clock fsm

/** return values */
typedef enum return_values
{
    RET_HANDLED,   //< event was handled
    RET_IGNORED,   //< event was ignored; not used in this implementation
    RET_TRANSITION //< event was handled and a state transition occurred
} fsm_return_status_t;

// Event Signals
typedef enum
{
    ENTRY,
    EXIT,
    PUSHBUTTON_PRESSED,
    ROTARYBUTTON_PRESSED,
    TIME_MATCH,
    TIMEOUT
} signal_t;

//  Function Pointer for states
typedef fsm_return_status_t (*state_t)(fsm_t *, const event_t *);

// FSM Structure
struct fsm_s
{
    state_t state;       //< current state, pointer to the state handler functions
    bool isAlarmEnabled; //< flag for the alarm status
    time_t timeSet;      //< multi-purpose var for system or alarm time
    time_t alarmTime;
};

// Event Structure
typedef struct event_s
{
    uint8_t signal; //< identifies the type of event
} event_t;

// --- State Function Declarations ---
fsm_return_status_t state_uninitialized_setHour(fsm_t *fsm, const event_t *e);
fsm_return_status_t state_uninitialized_setMinute(fsm_t *fsm, const event_t *e);
fsm_return_status_t state_normalOperation(fsm_t *fsm, const event_t *e);
fsm_return_status_t state_setAlarm_hour(fsm_t *fsm, const event_t *e);
fsm_return_status_t state_setAlarm_minute(fsm_t *fsm, const event_t *e);
fsm_return_status_t state_alarmActive(fsm_t *fsm, const event_t *e);

// --- FSM Dispatcher & Initialization ---
void fsm_dispatch(fsm_t *fsm, const event_t *e);
void fsm_init(fsm_t *fsm, state_t initialState);

#endif /* FSM_H_ */