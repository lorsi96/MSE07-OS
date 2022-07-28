/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include "main.h"

#include "MyOs_Core.h"
#include "MyOs_TestTasks.h"
#include "board.h"
#include "sapi.h"

/* ************************************************************************* */
/*                           Macros and Definitions                          */
/* ************************************************************************* */
#define MY_OS_MILLIS 1000

/* ************************************************************************* */
/*                       Private Functions Definitions                       */
/* ************************************************************************* */

/**
 * @brief hardware initialization function
 */
static void initHardware(void) {
    Board_Init();
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / MY_OS_MILLIS);
}

/* ************************************************************************* */
/*                             Tasks Definitions                             */
/* ************************************************************************* */
void sillyCountTask(void* _countStep) {
    uint32_t countStep = (uint32_t)_countStep;
    volatile int i = 0;
    volatile int h = 0;  // volatile to avoid comp. optimization.
    for (;;) {
        i += countStep;
        h += countStep;
        MyOs_blockTask(NULL);
    }
}

void sillyFPUCountTask(void* _countStep) {
    float countStep = ((uint32_t)_countStep) / 10.;
    volatile float i = 0.0;
    volatile float h = 0.0;  // volatile to avoid comp. optimization.
    for (;;) {
        i += countStep;
        h += countStep;
    }
}

/* ************************************************************************* */
/*                              Main Definition                              */
/* ************************************************************************* */

int main(void) {
    initHardware();

    MyOs_initialize();
    MyOS_taskCreate(sillyCountTask, /*parameters=*/(void*)3, /*handle=*/NULL);
    //MyOS_taskCreate(sillyCountTask, /*parameters=*/(void*)4, /*handle=*/NULL);
    //MyOS_taskCreate(sillyCountTask, /*parameters=*/(void*)2, /*handle=*/NULL);
    MyOS_taskCreate(sillyFPUCountTask, /*parameters=*/(void*)1, /*handle=*/NULL);

    for (;;) {
    }
}
