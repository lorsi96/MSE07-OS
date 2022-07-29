/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include "main.h"

#include "MyOs_Core.h"
#include "MyOs_Event.h"
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


MyOs_Event_t myEvent;

/* ************************************************************************* */
/*                             Tasks Definitions                             */
/* ************************************************************************* */
void waitingTask(void* _) {
    MyOs_eventWait(&myEvent, 0x01);
    gpioWrite(LEDG, true);
    for(;;);
}

void buttonTask(void* _) {
    while(gpioRead(TEC1));
    MyOs_eventPost(&myEvent, 0x01);
    for(;;);
}


/* ************************************************************************* */
/*                              Main Definition                              */
/* ************************************************************************* */

int main(void) {
    initHardware();

    MyOs_createEvent(&myEvent);
    
    MyOs_initialize();
    MyOS_taskCreate(waitingTask, /*parameters=*/NULL, /*handle=*/NULL);
    MyOS_taskCreate(buttonTask, /*parameters=*/NULL, /*handle=*/NULL);

    for (;;);
}
