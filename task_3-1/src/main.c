#include <avr/io.h>
#include <avr/interrupt.h>
#include "ses_button.c"
#include "ses_led.c"
#include <util/delay.h>
#include "ses_usbserial.h"

void onPushButtonPressed(void) {
    led_redToggle();
}

void onRotaryButtonPressed(void) {
    led_greenToggle();
}

int main(void) {
    led_redInit();
    led_greenInit();
    button_init(false);

    sei();

    button_setPushButtonCallback(onPushButtonPressed);
    button_setRotaryButtonCallback(onRotaryButtonPressed);

    while(1){
        _delay_ms(100);
    }

    return 0;
}
