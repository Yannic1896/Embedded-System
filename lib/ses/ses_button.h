#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include <stdbool.h>

/* FUNCTION PROTOTYPES *******************************************************/

typedef void (*pButtonCallback)(void);      // Function pointer

/**
 * Initializes rotary encoder button and pushbutton
 */
void button_init(bool useDebounce);

/** 
 * Get the state of the pushbutton.
 */
bool button_isPushButtonPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryButtonPressed(void);

void button_setRotaryButtonCallback(pButtonCallback callback);

void button_setPushButtonCallback(pButtonCallback callback);

void button_checkState(void);

#endif /* SES_BUTTON_H_ */
