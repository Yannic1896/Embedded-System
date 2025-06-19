#ifndef PSCHEDULER_H
#define PSCHEDULER_H

#include <stdint.h>

// Task function pointer type
typedef void (*task_t)(void);

// Initialize and run the scheduler with the given tasks
void pscheduler_run(const task_t *taskArray, uint8_t len);

#endif