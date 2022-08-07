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

    uartConfig( UART_USB, 115200 );
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
MyOs_queue_CREATE_STATIC(uartQueue, char, 5);
MyOs_semaphore_CREATE_STATIC(mySemaphore);


/* ************************************************************************* */
/*                          Utilities for Test Tasks                         */
/* ************************************************************************* */
typedef struct {
    uint8_t inv_id;
    uint8_t sensor_id; 
    uint32_t periodMs;
} SensorConfig_t;

void humiditySensorTask(void* sensorCfg) {
    SensorConfig_t* _sensorCfg = sensorCfg;
    char MSG_TEMPLATE[] = "[INVERNADERO:00][HUM:1][55\045]\n\r"; 
    MSG_TEMPLATE[14] = _sensorCfg->sensor_id + '0';
    MSG_TEMPLATE[21] = _sensorCfg->inv_id + '0';
    char buffer[sizeof(MSG_TEMPLATE)];
    uint8_t hum = 0;
    for(;;) {
        hum = (hum + 7) % 100;
        MyOs_enterCritical();
        uartWriteByteArray(UART_USB, MSG_TEMPLATE, sizeof(MSG_TEMPLATE));
        MyOs_exitCritical();
        MyOs_taskDelay(_sensorCfg->periodMs);
    }   
}

void temperatureSensorTask(void* sensorCfg) {
    SensorConfig_t* _sensorCfg = sensorCfg;
    char MSG_TEMPLATE[] = "[INVERNADERO:00][TEMP:1][27.5 C]\n\r"; 
    MSG_TEMPLATE[14] = _sensorCfg->sensor_id + '0';
    MSG_TEMPLATE[22] = _sensorCfg->inv_id + '0';
    char buffer[sizeof(MSG_TEMPLATE)];
    uint8_t hum = 0;
    for(;;) {
        hum = (hum + 7) % 100;
        MyOs_enterCritical();
        uartWriteByteArray(UART_USB, MSG_TEMPLATE, sizeof(MSG_TEMPLATE));
        MyOs_exitCritical();
        MyOs_taskDelay(_sensorCfg->periodMs);
    }   
}

/* ************************************************************************* */
/*                              Main Definition                              */
/* ************************************************************************* */

int main(void) {
    const SensorConfig_t hum1 = {
        .inv_id = 1,
        .sensor_id = 1,
        .periodMs = 100
    };

    const SensorConfig_t hum2 = {
        .inv_id = 2,
        .sensor_id = 2,
        .periodMs = 200
    };

    const SensorConfig_t temp2 = {
        .inv_id = 3,
        .sensor_id = 1,
        .periodMs = 250
    };

    initHardware();
    MyOs_initialize();

    MyOS_taskCreate(humiditySensorTask, &hum1, 3, NULL);
    MyOS_taskCreate(humiditySensorTask, &hum2, 3, NULL);
    MyOS_taskCreate(temperatureSensorTask, &temp2, 3, NULL);

    for (;;)
        ;
}
