#include <avr/io.h>
#include <avr/interrupt.h>
#include "ses_button.c"
#include "ses_led.c"
#include <util/delay.h>
#include "ses_usbserial.h"

void onPushButtonPressed(void) {
    led_redToggle();
    fprintf(serialout, "red led toggled");
}

void onRotaryButtonPressed(void) {
    led_greenToggle();
    fprintf(serialout, "green led toggled");
}

int main(void) {
    led_redInit();
    led_greenInit();
    button_init();
    usbserial_init();
    sei();

    button_setPushButtonCallback(onPushButtonPressed);
    button_setRotaryButtonCallback(onRotaryButtonPressed);

    while(1){
        _delay_ms(100);
    }

    return 0;
}
