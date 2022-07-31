/**
 * @file MyOs_Hooks.h
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-31
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __MY_OS_HOOKS__
#define __MY_OS_HOOKS__

/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include "MyOs_Types.h"

/* ************************************************************************* */
/*                                   Macros                                  */
/* ************************************************************************* */
#define __weak __attribute__((weak))

/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */
/**
 * @brief Task return hook
 *
 * @param returningTask handle of the task that returned.
 * @details Called only in case a task does return, which should never happen.
 *
 */
void MyOs_returnHook(MyOs_TaskHandle_t returningTask);

/**
 * @brief System tick hook.
 *
 * @details Called from within Systick every time a system-tick event happens.
 *
 */
void MyOs_tickHook();

/**
 * @brief Called whenever an Os error occurs.
 *
 * @param caller function where the error was risen.
 * @param err error type.
 */
void MyOs_errorHook(void* caller, MyOs_Error_t err);

/**
 * @brief Called when the system memory is insuficcient to allocate a new obj.
 *
 * @param caller function whose allocation failed.
 */
void MyOs_memoryInsufficientHook(void* caller);

/**
 * @brief Idle task. Called only when no other task is available.
 *
 */
void MyOs_idleTask();

/* ************************************************************************* */

#endif  // __MY_OS_HOOKS__
