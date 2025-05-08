#include "ses_button.h"
#include <util/delay.h>
#include <avr/io.h>
#include "ses_led.h"
#include <stdio.h>
#include "ses_usbserial.h"
#include "ses_display.h"
#include "ses_adc.h"

int main(void) {

adc_init(); // Initialize ADC

int32_t adc = adc_read(ADC_TEMP_CH);
return (int16_t)(((adc - ADC_TEMP_RAW_LOW) * (ADC_TEMP_HIGH - ADC_TEMP_LOW))
/ (ADC_TEMP_RAW_HIGH - ADC_TEMP_RAW_LOW)) + ADC_TEMP_LOW;

    return 0;
}