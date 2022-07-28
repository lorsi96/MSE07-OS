#ifndef __MY_OS_HOOKS__
#define __MY_OS_HOOKS__
#define __weak __attribute__((weak))  

/**
 * @brief Task return hook
 * 
 * @details Called only in case a task does return, which should never happen.
 * 
 */
void MyOs_returnHook();

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
 * @brief Idle task. Called only when no other task is available.
 * 
 */
void MyOs_idleTask();

#endif // __MY_OS_HOOKS__

