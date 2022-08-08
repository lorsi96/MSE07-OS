/**
 * @file MyOs_Task.h
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-31
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __MY_OS_TASK__
#define __MY_OS_TASK__

/** @addtogroup MyOs_Task
 * @{
 */

/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include <stdint.h>

#include "MyOs_Kernel.h"
#include "MyOs_Types.h"

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

/**
 * @brief Updates a task priority
 *
 */
void MyOs_taskSetPriority(MyOs_TaskHandle_t taskHandle, uint8_t priority);

/**
 * @brief Suspends a given task.
 *
 * @param taskHandle task to be suspended.
 */
void MyOs_suspendTask(MyOs_TaskHandle_t taskHandle);

/**
 * @brief Resumes a given task.
 *
 * @param taskHandle task to be resumed.
 */
void MyOs_resumeTask(MyOs_TaskHandle_t taskHandle);

/* ************************************************************************* */
/*                        Public OS Internal Functions                       */
/* ************************************************************************* */

/**
 * @brief Blocks a given task.
 *
 * @param taskHandle task to be blocked.
 */
void MyOs_blockTask(MyOs_TaskHandle_t taskHandle, uint32_t msToWait);

/**
 * @brief Unblocks a given task.
 *
 * @param taskHandle task to be unblocked.
 */
void MyOs_unblockTask(MyOs_TaskHandle_t taskHandle);

/**
 * @}
 */

/* ************************************************************************* */
#endif  // __MY_OS_TASK__