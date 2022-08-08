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
/*                                 DataTypes                                 */
/* ************************************************************************* */
typedef enum {
    SENSOR_TYPE_HUM,
    SENSOR_TYPE_TMP,
} SensorType_t;

typedef struct {
    uint8_t greenhouseId;
    uint8_t sensorId; 
    uint32_t periodMs;
    SensorType_t type;
} SensorConfig_t;

typedef struct {
    SensorType_t type;
    uint8_t greenhouseId;
    uint8_t sensorId;
    uint32_t sensorData;
} SensorData_t;

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

	Chip_SCU_GPIOIntPinSel( 2, TEC2_PORT_NUM, TEC2_BIT_VAL );
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 2 ) );
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 2 ) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( 2 ) );

    uartConfig( UART_USB, 115200 );
}


/* ************************************************************************* */
/*                              Sync Primitives                              */
/* ************************************************************************* */
MyOs_Event_t taskSuspentionEvent;
MyOs_TaskHandle_t humidityTaskHandle;
MyOs_TaskHandle_t temperatureTaskHandle;
MyOs_queue_CREATE_STATIC(myQueue, uint32_t, 5);
MyOs_queue_CREATE_STATIC(sensorDataQueue, SensorData_t, 5);
MyOs_queue_CREATE_STATIC(uartQueue, char, 5);
MyOs_semaphore_CREATE_STATIC(mySemaphore);



void tec1DownIsr() {
    MyOs_eventPost(&taskSuspentionEvent, 0b10);
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 0 ) );
}


void tec2DownIsr() {
    MyOs_eventPost(&taskSuspentionEvent, 0b01);
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 2 ) );
}

/* ******************************* Uart Task ******************************* */
void uartSenderTask(void* _) {
    char item;
    for(;;) {
        MyOs_queueReceive(&uartQueue, &item, MY_OS_MAX_DELAY);
        uartWriteByte(UART_USB, item);
    }
}

void sensorDataSerializerTask(void* _) {
    char MSG_HUM_BASE[] = "[GREENHOUSE:00?][HUM:0?][??\045]\n\r"; 
    char MSG_TMP_BASE[] = "[GREENHOUSE:00?][TEMPERATURE:0?][?? C]\n\r";
    char* currentMsg = NULL;
    uint8_t currentMsgLen = 0;
    SensorData_t sensorData;
    for(;;) {
        MyOs_queueReceive(&sensorDataQueue, &sensorData, MY_OS_MAX_DELAY);
        if(sensorData.type == SENSOR_TYPE_HUM) {
            currentMsg = MSG_HUM_BASE;
            currentMsgLen = sizeof(MSG_HUM_BASE);
            MSG_HUM_BASE[14] = sensorData.greenhouseId + '0';
            MSG_HUM_BASE[22] = sensorData.sensorId + '0';
            MSG_HUM_BASE[25] = (sensorData.sensorData % 100) / 10 + '0';
            MSG_HUM_BASE[26] = sensorData.sensorData % 10 + '0';
        }
        if(sensorData.type == SENSOR_TYPE_TMP) {
            currentMsg = MSG_TMP_BASE;
            currentMsgLen = sizeof(MSG_TMP_BASE);
            MSG_TMP_BASE[14] = sensorData.greenhouseId + '0';
            MSG_TMP_BASE[30] = sensorData.sensorId + '0';
            MSG_TMP_BASE[33] = (sensorData.sensorData % 100) / 10 + '0';
            MSG_TMP_BASE[34] = sensorData.sensorData % 10 + '0';
        }
        for(char i=0; currentMsg[i] !='\0'; i++) {
            MyOs_queueSend(&uartQueue, &currentMsg[i]);
        }
    }
}

void tasksSuspenderTask(void* _) {
    bool isTaskSuspended[] = {false, false};
    for(;;) {
        MyOs_eventWait(&taskSuspentionEvent, 0b111, MY_OS_MAX_DELAY);
        if(taskSuspentionEvent.flags & 0b001) {
            isTaskSuspended[0] ? MyOs_resumeTask(humidityTaskHandle) : MyOs_suspendTask(humidityTaskHandle);
            isTaskSuspended[0] = !isTaskSuspended[0];
        }
        if(taskSuspentionEvent.flags & 0b010) {
            isTaskSuspended[1] ? MyOs_resumeTask(temperatureTaskHandle) : MyOs_suspendTask(temperatureTaskHandle);
            isTaskSuspended[1] = !isTaskSuspended[1];
        }
        MyOs_eventSet(&taskSuspentionEvent, 0b000);
    }
}


/* ************************************************************************* */
/*                          Utilities for Test Tasks                         */
/* ************************************************************************* */
void sensorTask(void* sensorCfg) {
    SensorConfig_t* _sensorCfg = sensorCfg;
    SensorData_t data;
    data.greenhouseId = _sensorCfg->greenhouseId;
    data.sensorId = _sensorCfg->sensorId;
    data.type = _sensorCfg->type;
    for(;;) {
        data.sensorData = 50;
        MyOs_queueSend(&sensorDataQueue, &data);
        MyOs_taskDelay(_sensorCfg->periodMs);
    }   
}

/* ************************************************************************* */
/*                              Main Definition                              */
/* ************************************************************************* */

int main(void) {
    const SensorConfig_t hum1 = {
        .greenhouseId = 1,
        .sensorId = 1,
        .periodMs = 100,
        .type = SENSOR_TYPE_HUM
    };

    const SensorConfig_t hum2 = {
        .greenhouseId = 2,
        .sensorId = 2,
        .periodMs = 200,
        .type = SENSOR_TYPE_HUM
    };

    const SensorConfig_t temp2 = {
        .greenhouseId = 3,
        .sensorId = 1,
        .periodMs = 250,
        .type = SENSOR_TYPE_TMP
    };

    MyOs_eventCreate(&taskSuspentionEvent);

    initHardware();
    MyOs_initialize();
    MyOS_taskCreate(uartSenderTask, NULL, 3, NULL);
    MyOS_taskCreate(sensorDataSerializerTask, NULL, 3, NULL);
    MyOS_taskCreate(tasksSuspenderTask, NULL, 3, NULL);
    MyOS_taskCreate(sensorTask, &hum2, 3, &humidityTaskHandle);
    MyOS_taskCreate(sensorTask, &temp2, 3, &temperatureTaskHandle);
        
    MyOs_installIRQ(PIN_INT0_IRQn, tec1DownIsr);
    MyOs_installIRQ(PIN_INT2_IRQn, tec2DownIsr);

    for (;;)
        ;
}
