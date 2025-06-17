#include "ses_led.h"
#include <util/delay.h>

int main(void){
    led_redInit(); // Initialize red LED
    led_yellowInit(); // Initialize yellow LED
    led_greenInit(); // Initialize green LED

    while (1)
    {
        led_redOn(); // Turn on red LED
        led_yellowToggle(); // Toggle yellow LED
        led_greenOff(); // Turn off green LED

        _delay_ms(1000); // Wait for 1 second
    }
    
    
    return 0;
}  