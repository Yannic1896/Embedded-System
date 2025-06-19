/* INCLUDES *******************************************************************/
#include <stdlib.h>

#include "ses_timer.h"
#include "ses_scheduler.h"
#include "util/atomic.h"


/* PRIVATE VARIABLES *********************************************************/

/**
 * We make sure that the list is accessed only within atomic sections
 * protected by a memory barrier --> no volatile necessary
 */
static task_descriptor_t * taskList = NULL;


/* FUNCTION DEFINITION *************************************************/

static void scheduler_update(void) {
    task_descriptor_t* current = taskList;
    
    while (current != NULL) {
        if (current->expire > 0) {
            current->expire--;
            
            if (current->expire == 0) {
                current->execute = 1;
                
                // Reset expiration time for periodic tasks
                if (current->period > 0) {
                    current->expire = current->period;
                }
            }
        }
        current = current->next;
    }
}

void scheduler_init() {
    // Initialize timer with 1ms callback
    timer0_setCallback(&scheduler_update);
    timer0_start();
    sei();
}

void scheduler_run() {
    while (1) {
        task_descriptor_t *current = taskList;

        while (current != NULL) {
            uint8_t shouldExecute = 0;

            // Read and clear execute flag atomically
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                if (current->execute) {
                    current->execute = 0;
                    shouldExecute = 1;
                }
            }

            if (shouldExecute) {
                current->task(current->param);  // Run the task

                if (current->period > 0) {
                    // Reschedule periodic task
                    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                        current->expire = current->period;
                    }
                } else {
                    // One-shot task — remove it
                    task_descriptor_t *toRemove = current;
                    current = current->next;  // Save next before removal
                    scheduler_remove(toRemove);
                    continue;  // Skip current = current->next;
                }
            }

            current = current->next;
        }
    }
}

bool scheduler_add(task_descriptor_t * toAdd) {

    //Check if task is invalid
    if (toAdd == NULL || toAdd->task == NULL){
        return false;
    }

    // Check if task is alreadey in task list
    task_descriptor_t *current = taskList;
    while (current != NULL){
        if (current == toAdd){
            return false;       // Task already in list
        }
        current = current->next;
    }

    // Initialize internal fields
    toAdd->execute = 0;
    toAdd->next = NULL;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        toAdd->next = taskList;
        taskList = toAdd;
    }

    return true;
}

void scheduler_remove(const task_descriptor_t * toRemove) {
    if (toRemove == NULL) {
        return;
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        task_descriptor_t *current = taskList;
        task_descriptor_t *prev = NULL;

        while (current != NULL) {
            if (current == toRemove) {
                if (prev == NULL) {
                    // Task is at the head
                    taskList = current->next;
                } else {
                    prev->next = current->next;
                }
                break;  // Task removed
            }

            prev = current;
            current = current->next;
        }
    }
}