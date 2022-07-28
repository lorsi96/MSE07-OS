/**
 * @file MyOs_Core.h
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-16
 *
 * @copyright Copyright (c) 2022
 *
 */
/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#ifndef __MY_OS_CORE__
#define __MY_OS_CORE__

#include <stdint.h>

#include "MyOs_Types.h"
#include "MysOs_Config.h"

/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */

/**
 * @brief Initializes the operative system.
 *
 */
void MyOs_initialize();

/**
 * @brief Creates a task.
 *
 * @param[in] taskCode function that implements the task.
 * @param[in] parameters parameters passed into the task.
 * @param[out] handle used to pass out the created task's handle.
 */
void MyOS_taskCreate(const void* taskCode, void* parameters,
                             MyOs_TaskHandle_t* handle);

#endif  // __MY_OS_CORE__
