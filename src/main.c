/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include "main.h"

#include "MyOs_Core.h"
#include "MyOs_Event.h"
// #include "MyOs_TestTasks.h"
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
    for(;;) {
        MyOs_eventWait(&myEvent, 0b11);
        if(myEvent.flags & 0b01) {
            gpioToggle(LEDG); 
        }
        if(myEvent.flags & 0b10) {
            gpioToggle(LEDR); 
        }
        MyOs_eventSet(&myEvent, 0x00);
    }
}

void buttonTaskA(void* flag) {
    for(;;) {
        while(gpioRead(TEC1)); // No debounce... but serves its purpose as is.
        MyOs_eventPost(&myEvent, 0b01);
    }
}

void buttonTaskB(void* flag) {
    for(;;) {
        while(gpioRead(TEC2)); // No debounce... but serves its purpose as is.
        MyOs_eventPost(&myEvent, 0b10);
    }
}

/* ************************************************************************* */
/*                              Main Definition                              */
/* ************************************************************************* */

int main(void) {
    initHardware();

    MyOs_createEvent(&myEvent);
    
    MyOs_initialize();
    MyOS_taskCreate(waitingTask, /*parameters=*/NULL, 2, /*handle=*/NULL);
    MyOS_taskCreate(buttonTaskA, /*parameters=*/NULL, 1,/*handle=*/NULL);
    MyOS_taskCreate(buttonTaskB, /*parameters=*/NULL, 2,/*handle=*/NULL);

    for (;;);
}
