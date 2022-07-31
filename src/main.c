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
        if(myEvent.flags & 0b100) {
            gpioToggle(LEDB); 
        }
        MyOs_eventSet(&myEvent, 0x00);
    }
}

uint16_t __pkg_tec_ev(uint8_t tec, uint8_t evt) {
    return (tec << 8) | evt;
}

void __unpack_tec_ev(void* packed, uint8_t* tec, uint8_t* evt) {
    *tec = (uint16_t)packed >> 8;
    *evt = (uint16_t)packed & 0xFF;
}


void buttonTask(void* buttonEvt) {
    uint8_t tec, evt;
    __unpack_tec_ev(buttonEvt, &tec, &evt);
    for(;;) {
        while(gpioRead(tec)); // No debounce... but serves its purpose as is.
        MyOs_eventPost(&myEvent, evt);
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
    MyOS_taskCreate(
        buttonTask, 
        /*parameters=*/(void*)__pkg_tec_ev(TEC1, 0b001), 
        /*priority=*/2,
        /*handle=*/NULL
    );
    MyOS_taskCreate(
        buttonTask, 
        /*parameters=*/(void*)__pkg_tec_ev(TEC2, 0b010), 
        /*priority=*/2,
        /*handle=*/NULL
    );
    MyOS_taskCreate(
        buttonTask, 
        /*parameters=*/(void*)__pkg_tec_ev(TEC3, 0b100), 
        /*priority=*/1, // Won't run!
        /*handle=*/NULL
    );
    for (;;);
}
