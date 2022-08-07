/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include "main.h"

#include "sapi.h"

#include "MyOs_Event.h"
#include "MyOs_Queue.h"
#include "MyOs_Semaphore.h"
#include "MyOs_Task.h"
#include "MyOs_Types.h"
#include "MyOs_ISR.h"
#include "board.h"

/* ************************************************************************* */
/*                           Macros and Definitions                          */
/* ************************************************************************* */
#define MY_OS_MILLIS 1000

#define TEC1_PORT_NUM   0
#define TEC1_BIT_VAL    4

#define TEC2_PORT_NUM   0
#define TEC2_BIT_VAL    8

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

	Chip_SCU_GPIOIntPinSel( 0, TEC1_PORT_NUM, TEC1_BIT_VAL );
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 0 ) ); // INT0 flanco descendente
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 0 ) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( 0 ) );

	Chip_SCU_GPIOIntPinSel( 1, TEC1_PORT_NUM, TEC1_BIT_VAL );
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 1 ) ); // INT1 flanc
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 1 ) );
	Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( 1 ) );
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


void button1ISR() {
    MyOs_eventPost(&myEvent, 0b001);
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 0 ) );
}


void button2ISR() {
    MyOs_eventPost(&myEvent, 0b010);
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 1 ) );
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
    MyOs_installIRQ(PIN_INT0_IRQn, button1ISR);
    MyOs_installIRQ(PIN_INT1_IRQn, button2ISR);
    for (;;)
        ;
}
