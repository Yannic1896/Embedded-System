#include "ses_adc.h"
#include <avr/io.h>


void adc_init(void) {
    // Configure the data direction registers for potentiometer, temperature, and light sensors
    DDRC &= ~((1 << ADC_LIGHT_CH) | (1 << ADC_POTI_CH) | (1 << ADC_TEMP_CH));

    // Deactivate internal pull-up resistors
    PORTC &= ~((1 << ADC_LIGHT_CH) | (1 << ADC_POTI_CH) | (1 << ADC_TEMP_CH));

    // Disable power reduction mode for the ADC module
    PRR0 &= ~(1 << PRADC);

    // Select external reference voltage (3.3V connected to AREF pin) and set ADC result right-adjusted
    ADMUX = (0 << REFS1) | (0 << REFS0) | (0 << ADLAR); // External reference, right-adjusted

    // Enable ADC and set prescaler using the ADC_PRESCALE macro
    ADCSRA = (1 << ADEN) | ADC_PRESCALE;

    // Do not enable auto triggering (ADATE is cleared by default)
}

/**
 * Read the raw ADC value of the given channel
 * @adc_channel The channel as element of the ADCChannels enum
 * @return The raw ADC value
 */
uint16_t adc_read(uint8_t adc_channel) {
    // Check if the provided channel is valid
    if (adc_channel > ADC_MAX_CHANNEL) {
        return ADC_INVALID_CHANNEL; // Return invalid channel error
    }

    // Select the ADC channel by setting the lower bits of ADMUX
    ADMUX = (ADMUX & 0xF0) | (adc_channel & 0x0F);

    // Start the ADC conversion by setting the ADSC bit in ADCSRA
    ADCSRA |= (1 << ADSC);

    // Wait for the conversion to complete (polling the ADSC bit)
    while (ADCSRA & (1 << ADSC)) {
        // Busy wait until ADSC is cleared by hardware
    }

    // Read the ADC result (ADCL must be read first, then ADCH)
    uint16_t result = ADCL; // Read low byte
    result |= (ADCH << 8);  // Read high byte and combine with low byte

    return result; // Return the raw ADC conversion result
}

/**
 * Read the current temperature
 * @return Temperature in tenths of degree celsius
 */
int16_t adc_getTemperature(void);


