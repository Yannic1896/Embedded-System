#include <avr/io.h>
#include <avr/interrupt.h>
#include "ses_button.h"
#include "ses_led.h"
#include <util/delay.h>

void onPushButtonPressed(void) {
    led_redToggle();
}

void onRotaryButtonPressed(void) {
    led_greenToggle();
}

int main(void) {
    led_init();
    button_init();
    sei();

    button_setPushButtonCallback(onPushButtonPressed);
    button_setRotaryButtonCallback(onRotaryButtonPressed);

    while(1){
        _delay_ms(100);
    }

    return 0;
}
