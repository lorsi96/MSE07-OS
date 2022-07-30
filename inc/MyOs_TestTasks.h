//#ifdef MY_OS_TEST_TASKS
#ifndef __MY_OS_TEST_TASKS__
#define __MY_OS_TEST_TASKS__

#include "sapi.h"
#include "MyOs_Types.h"
#include "MyOs_Core.h"

/* ******************************* Test Tasks ****************************** */
void MyOs_selfBlockTask(void* _) {
    MyOs_blockTask(0);
}

/* ********************************* Hooks ********************************* */
void MyOs_returnHook(MyOs_TaskHandle_t returningTask) {
    gpioWrite(LED2, true);
}

void MyOs_tickHook() {
    gpioToggle(LED1);
}

void MyOs_errorHook(void* caller, MyOs_Error_t err) {
    gpioWrite(LEDR, true);
    for(;;);
}


/* ******************************* Idle Task ******************************* */
void MyOs_idleTask() {
    gpioWrite(LED3, true);
    for(;;) {
        __asm("wfi");
    }
}

#endif
//#endif