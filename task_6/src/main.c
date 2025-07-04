#include <avr/io.h>
#include <util/delay.h>

typedef enum {
    STATE_UNINITIALIZED,
    STATE_SET_HOUR,
    STATE_SET_MINUTE,
    STATE_NORMAL,
    STATE_SET_ALARM_HOUR,
    STATE_SET_ALARM_MINUTE,
    STATE_ALARM_ACTIVE
} ClockState_t;