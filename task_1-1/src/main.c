#include <avr/io.h>
#include <util/delay.h>

/** toggles the red LED of the SES-board */
int main(void) {

	DDRF |= (1 << DDF5);

	while (1) {
		_delay_ms(1000);
		
		PORTF ^= (1 << PORTF5);
	}
	return 0;
}
