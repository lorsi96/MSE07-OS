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

/**
 * @brief Enters critical section.
 *
 */
void MyOs_enterCritical();

/**
 * @brief Exits critical section.
 *
 */
void MyOs_exitCritical();

/**
 * @brief Tells whether the current context is an ISR or not
 *
 * @return true if running an ISR, false otherwise.
 */
bool MyOs_isContextISR();

/**
 * @brief Runs code within inside a critical block.
 *
 */
#define MyOs_CRITICAL(code)   \
    do {                      \
        MyOs_enterCritical(); \
        code;                 \
        MyOs_exitCritical();  \
    } while (false);

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

/**
 * @brief Updates the operating system state.
 *
 * @param state state to switch to.
 */
void MyOs_updateState(MyOs_GeneralState_t state);

/* ************************************************************************* */

#endif  // __MY_OS_KERNEL__
