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
 * @return MyOs_Error_t
 */
MyOs_Error_t MyOS_taskCreate(const void* taskCode, void* parameters,
                             MyOs_TaskHandle_t* handle);

#endif  // __MY_OS_CORE__
