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
}

void scheduler_run() {
    while (1) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            task_descriptor_t* current = taskList;
            task_descriptor_t* prev = NULL;
            
            while (current != NULL) {
                if (current->execute) {
                    // Execute the task
                    current->task(current->param);
                    current->execute = 0;
                    
                    // Remove one-shot tasks
                    if (current->period == 0) {
                        task_descriptor_t* toRemove = current;
                        
                        if (prev == NULL) {
                            // First element in list
                            taskList = current->next;
                        } else {
                            prev->next = current->next;
                        }
                        
                        current = current->next;
                        free(toRemove);
                        continue;
                    }
                }
                prev = current;
                current = current->next;
            }
        }
    }
}

bool scheduler_add(task_descriptor_t * toAdd) {
    if (toAdd == NULL || toAdd->task == NULL) {
        return false;
    }
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Check if task already exists in the list
        task_descriptor_t* current = taskList;
        while (current != NULL) {
            if (current == toAdd) {
                return false;
            }
            current = current->next;
        }
        
        // Initialize task descriptor
        toAdd->execute = 0;
        toAdd->next = NULL;
        
        // Add to beginning of list
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
        task_descriptor_t* current = taskList;
        task_descriptor_t* prev = NULL;
        
        while (current != NULL) {
            if (current == toRemove) {
                if (prev == NULL) {
                    // First element in list
                    taskList = current->next;
                } else {
                    prev->next = current->next;
                }
                
                free(current);
                break;
            }
            
            prev = current;
            current = current->next;
        }
    }
}