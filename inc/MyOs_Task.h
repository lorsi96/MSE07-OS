#ifndef __MY_OS_TASK__
#define __MY_OS_TASK__

#include <stdint.h>

#include "MyOs_Types.h"
#include "MyOs_Kernel.h"

/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */

/**
 * @brief Creates a task.
 *
 * @param[in] taskCode function that implements the task.
 * @param[in] parameters parameters passed into the task.
 * @param[out] handle used to pass out the created task's handle.
 */
void MyOS_taskCreate(const void* taskCode, void* parameters, uint8_t priority,
                             MyOs_TaskHandle_t* taskHandle);

/**
 * @brief Retrieves the current task handler.
 * 
 * @return MyOs_TaskHandle_t 
 */
MyOs_TaskHandle_t MyOs_getCurrentTask();

/**
 * @brief Block current task for a given number of ticks.
 * 
 * @param ticks number of ticks to wait for.
 */
void MyOs_taskDelay(const uint32_t ticks);


#endif // __MY_OS_TASK__