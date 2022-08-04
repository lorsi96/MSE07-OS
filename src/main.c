/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include "main.h"

#include "MyOs_Event.h"
#include "MyOs_Queue.h"
#include "MyOs_Semaphore.h"
#include "MyOs_Task.h"
#include "MyOs_Types.h"
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

void MyOs_errorHook(void* caller, MyOs_Error_t err) {
    gpioWrite(LED3, 1);
    for (;;)
        ;
}



/* ************************************************************************* */
/*                              Sync Primitives                              */
/* ************************************************************************* */
MyOs_Event_t myEvent;
MyOs_queue_CREATE_STATIC(myQueue, uint32_t, 5);
MyOs_semaphore_CREATE_STATIC(mySemaphore);


/* ************************************************************************* */
/*                          Utilities for Test Tasks                         */
/* ************************************************************************* */

/* *********** Functions for packing/unpacking buttons and events ********** */
uint16_t __pkg_tec_ev(uint8_t tec, uint8_t evt) { return (tec << 8) | evt; }

void __unpack_tec_ev(void* packed, uint8_t* tec, uint8_t* evt) {
    *tec = (uint16_t)packed >> 8;
    *evt = (uint16_t)packed & 0xFF;
}

#define __array_to_queue_args(arr)  sizeof(arr)/sizeof(arr[0]), sizeof(arr[0]), arr 

/* ************************************************************************* */
/*                             Tasks Definitions                             */
/* ************************************************************************* */

/* ************************** Events Testing Tasks ************************* */

/**
 * @brief Turns on LEDs based on external events.
 */
void eventConsumerTask(void* _) {
    for (;;) {
        MyOs_eventWait(&myEvent, 0b111);
        if (myEvent.flags & 0b001) {
            gpioToggle(LEDG);
        }
        if (myEvent.flags & 0b010) {
            gpioToggle(LEDR);
        }
        if (myEvent.flags & 0b100) {
            gpioToggle(LEDB);
        }
        MyOs_eventSet(&myEvent, 0x000);
    }
}

/**
 * @brief Produces events based on hardware key presses.
 * 
 * @param buttonEvt Button/Event pair, @see  __pkg_tec_ev
 */
void buttonTask(void* buttonEvt) {
    uint8_t tec, evt;
    __unpack_tec_ev(buttonEvt, &tec, &evt);
    for (;;) {
        while (gpioRead(tec))
            ;  // No debounce... but serves its purpose as is.
        MyOs_eventPost(&myEvent, evt);
    }
}


/* ************************** Queue Testing Tasks ************************** */

/**
 * @brief Requests leds to be turned on every 1 second using a queue.
 */
void blinkyRequesterTask(void* _) {
    uint8_t ledIndex = 0; // 0 LED1, 1 LED2.
    for(;;) {
        ledIndex = (ledIndex + 1) % 2;
        MyOs_queueSend(&myQueue, &ledIndex);
        MyOs_taskDelay(1000);
    }
}

/**
 * @brief Toggles LEDs based on messages read from a queue.
 */
void blinkyConsumerTask(void* _) {
    uint8_t msg;
    uint32_t leds[] = {LED1, LED2};
    for(;;) {
        MyOs_queueReceive(&myQueue, &msg);
        gpioToggle(leds[msg]);
    }
}

/* ************************ Semaphore Testing Tasks ************************ */

void blinkySemaphoreRequesterTask(void* _) {
    for(;;) {
        MyOs_semaphoreGive(&mySemaphore);
        MyOs_taskDelay(1000);
    }
}

void blinkySemaphoreConsumerTask(void* _) {
    for(;;) {
        MyOs_semaphoreTake(&mySemaphore);
        gpioToggle(LED3);
    }
}


/* ************************************************************************* */
/*                              Main Definition                              */
/* ************************************************************************* */

int main(void) {
    initHardware();

    MyOs_eventCreate(&myEvent);

    MyOs_initialize();
    MyOS_taskCreate(eventConsumerTask, /*parameters=*/NULL, 2, /*handle=*/NULL);
    MyOS_taskCreate(buttonTask,
                    /*parameters=*/(void*)__pkg_tec_ev(TEC1, 0b001),
                    /*priority=*/2,
                    /*handle=*/NULL);
    MyOS_taskCreate(buttonTask,
                    /*parameters=*/(void*)__pkg_tec_ev(TEC2, 0b010),
                    /*priority=*/2,
                    /*handle=*/NULL);
    MyOS_taskCreate(buttonTask,
                    /*parameters=*/(void*)__pkg_tec_ev(TEC3, 0b100),
                    /*priority=*/1,  // Won't run!
                    /*handle=*/NULL);
    MyOS_taskCreate(blinkyRequesterTask,
                    /*parameters=*/NULL,
                    /*priority=*/2,
                    /*handle=*/NULL);
    MyOS_taskCreate(blinkyConsumerTask,
                    /*parameters=*/NULL,
                    /*priority=*/2,
                    /*handle=*/NULL);
    MyOS_taskCreate(blinkySemaphoreRequesterTask,
                    /*parameters=*/NULL,
                    /*priority=*/2,
                    /*handle=*/NULL);
    MyOS_taskCreate(blinkySemaphoreConsumerTask,
                    /*parameters=*/NULL,
                    /*priority=*/2,
                    /*handle=*/NULL);
    for (;;)
        ;
}
