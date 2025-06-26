#ifndef SES_FANSPEED_H_
#define SES_FANSPEED_H_

#include <inttypes.h>
#include <avr/io.h>

void fanspeed_init(void);

uint16_t fanspeed_getRecent(void);

uint16_t fanspeed_getFiltered(void);

#endif