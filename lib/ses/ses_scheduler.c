/* INCLUDES *******************************************************************/
#include <stdlib.h>

#include "ses_timer.h"
#include "ses_usbserial.h"
#include "ses_scheduler.h"
#include "util/atomic.h"

/* PRIVATE VARIABLES *********************************************************/

/**
 * We make sure that the list is accessed only within atomic sections
 * protected by a memory barrier --> no volatile necessary
 */
static task_descriptor_t *taskList = NULL;

#define MS_PER_DAY 86400000UL // 24*60*60*1000
#define MS_PER_HOUR 3600000UL // 60*60*1000
#define MS_PER_MINUTE 60000UL // 60*1000
#define MS_PER_SECOND 1000UL  // 1000

static system_time_t systemTime = 0;

/* FUNCTION DEFINITION *************************************************/

static void scheduler_update(void)
{
    task_descriptor_t *current = taskList;
    // fprintf(serialout, "Scheduler");
    while (current != NULL)
    {
        if (current->expire > 0)
        {
            current->expire--; // Decrement the expire counter

            if (current->expire == 0)
            {
                current->execute = 1;

                // Reset expiration time for periodic tasks
                if (current->period > 0)
                {
                    current->expire = current->period;
                }
            }
        }
        current = current->next;
    }
    systemTime++; // Increase system time every ms
    if (systemTime > MS_PER_DAY)
    { // Reset after 24 hours
        systemTime = 0;
    }
}

void scheduler_init()
{
    // Initialize timer with 1ms callback
    timer0_setCallback(&scheduler_update);
    timer0_start();
    sei();
}

void scheduler_run()
{

    while (1)
    {
        task_descriptor_t *current = taskList;

        while (current != NULL)
        {
            uint8_t shouldExecute = 0;

            // Read and clear execute flag atomically
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                if (current->execute)
                {
                    current->execute = 0;
                    shouldExecute = 1;
                }
            }

            if (shouldExecute)
            {
                current->task(current->param); // Run the task

                if (current->period > 0)
                {
                    // Reschedule periodic task
                    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
                    {
                        current->expire = current->period;
                    }
                }
                else
                {
                    // Remove one-time task
                    task_descriptor_t *toRemove = current;
                    current = current->next; // Save next before removal
                    scheduler_remove(toRemove);
                    continue; // Skip current = current->next;
                }
            }

            current = current->next;
        }
    }
}

bool scheduler_add(task_descriptor_t *toAdd)
{

    // Check if task is invalid
    if (toAdd == NULL || toAdd->task == NULL)
    {
        return false;
    }

    // Check if task is already in task list
    task_descriptor_t *current = taskList;
    while (current != NULL)
    {
        if (current == toAdd)
        { // Task already in list
            return false;
        }
        current = current->next;
    }

    // Initialize internal fields
    toAdd->execute = 0;
    toAdd->next = NULL;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        toAdd->next = taskList;
        taskList = toAdd;
    }

    return true;
}

void scheduler_remove(const task_descriptor_t *toRemove)
{
    if (toRemove == NULL)
    {
        return;
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        task_descriptor_t *current = taskList;
        task_descriptor_t *prev = NULL;

        while (current != NULL)
        {
            if (current == toRemove)
            {
                if (prev == NULL)
                {
                    // Task is at the head
                    taskList = current->next;
                }
                else
                {
                    prev->next = current->next;
                }
                break; // Task removed
            }

            prev = current;
            current = current->next;
        }
    }
}

system_time_t scheduler_getTime(void)
{
    return systemTime;
}

void scheduler_setTime(system_time_t time)
{
    if (time < MS_PER_DAY)
    {
        systemTime = time;
    }
}
// Convert time_t to system_time_t
system_time_t timeToSystemTime(const time_t *t)
{
    return (t->hour * MS_PER_HOUR) +
           (t->minute * MS_PER_MINUTE) +
           (t->second * MS_PER_SECOND) +
           (t->milli);
}
// Convert system_time_t to time_t
void systemTimeToTime(system_time_t sysTime, time_t *t)
{
    t->hour = sysTime / MS_PER_HOUR;
    sysTime %= MS_PER_HOUR;

    t->minute = sysTime / MS_PER_MINUTE;
    sysTime %= MS_PER_MINUTE;

    t->second = sysTime / MS_PER_SECOND;
    sysTime %= MS_PER_SECOND;

    t->milli = sysTime;
}