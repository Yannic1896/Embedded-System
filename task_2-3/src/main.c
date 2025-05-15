#include "ses_usbserial.h"
#include "ses_display.h"
#include "ses_led.c"
#include "ses_button.c"
#include <util/delay.h>



int main(void) {
    usbserial_init();
    display_init();
    sei(); // Activate interrupts. Required to use USB serial communication.
    led_redInit();
    led_greenInit();
    button_init();

    uint8_t seconds = 0;

    while(1){
        /*
        fprintf(serialout, "Your USB output goes here\n");

        // Check Buttons
        if (button_isRotaryButtonPressed()) {
            led_redOn();
            fprintf(serialout, "Rotary pushed\n");
        } else {
            led_redOff();
            fprintf(serialout, "Rotary not pushed\n");
        }

        if (button_isPushButtonPressed()) {
            led_greenOn();
            fprintf(serialout, "Push pushed\n");
        } else {
            led_greenOff();
            fprintf(serialout, "Push not pushed\n");
        }
        
        // Display seconds since MCU reset
        display_setCursor(0,0); // Set cursor to column 0, row 0
        fprintf(displayout, "seconds: %d", seconds);
        
        display_update();
        */
        led_redOff();
        led_greenToggle();
        //led_greenOn();
        _delay_ms(500);
        /*
        static uint8_t halfSecondCounter = 0;
        halfSecondCounter++;
        if (halfSecondCounter >= 2) {
            seconds++;
            halfSecondCounter = 0;
        }*/

    }

    return 0;
}
