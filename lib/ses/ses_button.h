#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include <stdbool.h>

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder button and pushbutton
 */
void button_init(void);

/** 
 * Get the state of the pushbutton.
 */
bool button_isPushButtonPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryButtonPressed(void);


#endif /* SES_BUTTON_H_ */
