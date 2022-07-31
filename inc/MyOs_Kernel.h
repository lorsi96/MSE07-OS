/**
 * @file MyOs_Kernel.h
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-16
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __MY_OS_KERNEL__
#define __MY_OS_KERNEL__

/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
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
 * @brief Request a scheduler run & potential context switch.
 *
 */
void MyOs_yield();

/* ************************************************************************* */
/*                         Internals Public Functions                        */
/* ************************************************************************* */
/**
 * @brief Retrieves MyOs only instance.
 *
 * @return MyOs_t*
 */
MyOs_t* MyOs_getInstance();

/**
 * @brief Rises a MyOs error and calls the errorHook afterwards.
 *
 * @param caller caller function.
 * @param err error type.
 */
void MyOs_raiseError(void* caller, MyOs_Error_t err);

/* ************************************************************************* */

#endif  // __MY_OS_KERNEL__
