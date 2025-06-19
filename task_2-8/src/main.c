#include "ses_button.h"
#include <util/delay.h>
#include <avr/io.h>
#include "ses_led.h"
#include <stdio.h>
#include "ses_usbserial.h"
#include "ses_display.h"
#include "ses_adc.h"

int main(void) {
    usbserial_init();  // Initialize USB serial connection
    adc_init();        // Initialize ADC
    display_init();    // Initialize display

    uint16_t potentiometerValue; // Variable to store potentiometer value
    uint16_t lightValue;         // Variable to store light sensor value
    int16_t temperature;         // Variable to store temperature in tenths of a degree Celsius

    while (1) {
        // Read ADC values
        potentiometerValue = adc_read(ADC_POTI_CH); // Read potentiometer value
        lightValue = adc_read(ADC_LIGHT_CH);       // Read light sensor value
        temperature = adc_getTemperature();        // Read temperature in tenths of a degree Celsius

        // Display the sensor values on the screen
        display_setCursor(0, 0); // Set cursor to column 0, row 0
        fprintf(displayout, "POT: %u\n", potentiometerValue);
        fprintf(displayout, "Light: %u\n", lightValue);
        fprintf(displayout, "Temp: %d.%d C\n", temperature / 10, abs(temperature % 10));
        display_update(); // Update the display with the new content

        // Print the same values to the serial output
        fprintf(serialout, "POT: %u, Light: %u, Temp: %d.%d C\n", potentiometerValue, lightValue, temperature / 10, abs(temperature % 10));

        // Add a delay of 500 ms
        _delay_ms(500);
    }

    return 0;
}

