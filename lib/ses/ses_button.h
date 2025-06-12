#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include <stdbool.h>

/* FUNCTION PROTOTYPES *******************************************************/

typedef void (*pButtonCallback)(void);      // Function pointer

/**
 * Initializes rotary encoder button and pushbutton
 */
<<<<<<< HEAD
void button_init(bool debouncing);
=======
void button_init(bool useDebounce);
>>>>>>> Jannis

/** 
 * Get the state of the pushbutton.
 */
bool button_isPushButtonPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryButtonPressed(void);

<<<<<<< HEAD
typedef void (*pButtonCallback)(void);

//Callback functions
void button_setRotaryButtonCallback(pButtonCallback callback);
void button_setPushButtonCallback(pButtonCallback callback);

void button_checkState();
=======
void button_setRotaryButtonCallback(pButtonCallback callback);

void button_setPushButtonCallback(pButtonCallback callback);

void button_checkState(void);
>>>>>>> Jannis

#endif /* SES_BUTTON_H_ */
