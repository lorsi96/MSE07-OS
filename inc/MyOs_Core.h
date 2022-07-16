#ifndef __MY_OS_CORE__
#define ISO_I_2020_MSE_OS_INC_MSE_OS_CORE_H_

#include <stdint.h>
#include "MysOs_Config.h"
#include "MyOs_Types.h"


/**
 * @brief Initializes the operative system.
 * 
 */
void MyOs_initialize();

/**
 * @brief Creates a task.
 * 
 * @param[in] taskCode 
 * @param[out] handle 
 * @return MyOs_Error_t 
 */
MyOs_Error_t MyOS_taskCreate(const void *taskCode, MyOs_TaskHandle_t* handle);


#endif
