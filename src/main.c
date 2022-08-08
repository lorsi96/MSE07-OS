/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include "main.h"

#include "MyOs_Event.h"
#include "MyOs_ISR.h"
#include "MyOs_Queue.h"
#include "MyOs_Semaphore.h"
#include "MyOs_Task.h"
#include "MyOs_Types.h"
#include "board.h"
#include "sapi.h"
#include "sapi_rtc.h"

/* ************************************************************************* */
/*                           Macros and Definitions                          */
/* ************************************************************************* */
#define MY_OS_MILLIS 1000

#define TEC1_PORT_NUM 0
#define TEC1_BIT_VAL 4

#define TEC2_PORT_NUM 0
#define TEC2_BIT_VAL 8

#define HUM_SENSORS_N 2
#define TEMP_SENSORS_N 3

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

    Chip_SCU_GPIOIntPinSel(0, TEC1_PORT_NUM, TEC1_BIT_VAL);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT,
                               PININTCH(0));  // INT0 flanco descendente
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(0));
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(0));

    Chip_SCU_GPIOIntPinSel(1, TEC1_PORT_NUM, TEC1_BIT_VAL);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));  // INT1 flanc
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(1));
    Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(1));

    Chip_SCU_GPIOIntPinSel(2, TEC2_PORT_NUM, TEC2_BIT_VAL);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(2));
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(2));

    uartConfig(UART_USB, 115200);
}

/* ************************************************************************* */
/*                              Sync Primitives                              */
/* ************************************************************************* */
MyOs_Event_t taskSuspentionEvent;
MyOs_TaskHandle_t humidityTaskHandles[HUM_SENSORS_N];
MyOs_TaskHandle_t temperatureTaskHandles[TEMP_SENSORS_N];
SensorConfig_t hum[HUM_SENSORS_N];
SensorConfig_t temp[TEMP_SENSORS_N];

MyOs_queue_CREATE_STATIC(sensorDataQueue, SensorData_t, 5);
MyOs_queue_CREATE_STATIC(uartQueue, char, 5);

void tec1DownIsr() {
    MyOs_eventPost(&taskSuspentionEvent, 0b10);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
}

void tec2DownIsr() {
    MyOs_eventPost(&taskSuspentionEvent, 0b01);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
}

/* ******************************* Uart Task ******************************* */
void uartSenderTask(void* _) {
    char item;
    for (;;) {
        MyOs_queueReceive(&uartQueue, &item, MY_OS_MAX_DELAY);
        uartWriteByte(UART_USB, item);
    }
}

void sensorDataSerializerTask(void* _) {
    rtc_t someRtc;
    char MSG_TIME_BASE[] = "[??:??:??]";
    char MSG_HUM_BASE[] = "[GREENHOUSE:00?][HUM:0?][??\045]\n\r";
    char MSG_TMP_BASE[] = "[GREENHOUSE:00?][TEMPERATURE:0?][?? C]\n\r";
    char* currentMsg = NULL;
    uint8_t currentMsgLen = 0;
    SensorData_t sensorData;
    for (;;) {
        rtcRead(&someRtc);
        MSG_TIME_BASE[7] = (someRtc.sec % 100) / 10 + '0';
        MSG_TIME_BASE[8] = (someRtc.sec % 10) + '0';
        MSG_TIME_BASE[4] = (someRtc.min % 100) / 10 + '0';
        MSG_TIME_BASE[5] = (someRtc.min % 10) + '0';
        MSG_TIME_BASE[1] = (someRtc.hour % 100) / 10 + '0';
        MSG_TIME_BASE[2] = (someRtc.hour % 10) + '0';
        MyOs_queueReceive(&sensorDataQueue, &sensorData, MY_OS_MAX_DELAY);
        if (sensorData.type == SENSOR_TYPE_HUM) {
            currentMsg = MSG_HUM_BASE;
            currentMsgLen = sizeof(MSG_HUM_BASE);
            MSG_HUM_BASE[14] = sensorData.greenhouseId + '0';
            MSG_HUM_BASE[22] = sensorData.sensorId + '0';
            MSG_HUM_BASE[25] = (sensorData.sensorData % 100) / 10 + '0';
            MSG_HUM_BASE[26] = sensorData.sensorData % 10 + '0';
        }
        if (sensorData.type == SENSOR_TYPE_TMP) {
            currentMsg = MSG_TMP_BASE;
            currentMsgLen = sizeof(MSG_TMP_BASE);
            MSG_TMP_BASE[14] = sensorData.greenhouseId + '0';
            MSG_TMP_BASE[30] = sensorData.sensorId + '0';
            MSG_TMP_BASE[33] = (sensorData.sensorData % 100) / 10 + '0';
            MSG_TMP_BASE[34] = sensorData.sensorData % 10 + '0';
        }
        for (char i = 0; MSG_TIME_BASE[i] != '\0'; i++) {
            MyOs_queueSend(&uartQueue, &MSG_TIME_BASE[i]);
        }
        for (char i = 0; currentMsg[i] != '\0'; i++) {
            MyOs_queueSend(&uartQueue, &currentMsg[i]);
        }
    }
}

static inline void applyToHumiditySensorsTasks(
    void (*func)(MyOs_TaskHandle_t)) {
    for (uint8_t i = 0; i < HUM_SENSORS_N; i++) {
        func(humidityTaskHandles[i]);
    }
}

static inline void applyToTemperatureSensorsTasks(
    void (*func)(MyOs_TaskHandle_t)) {
    for (uint8_t i = 0; i < TEMP_SENSORS_N; i++) {
        func(temperatureTaskHandles[i]);
    }
}

void tasksSuspenderTask(void* _) {
    bool isTaskGroupSuspended[] = {false, false};
    for (;;) {
        MyOs_eventWait(&taskSuspentionEvent, 0b111, MY_OS_MAX_DELAY);
        if (taskSuspentionEvent.flags & 0b001) {
            isTaskGroupSuspended[0]
                ? applyToHumiditySensorsTasks(MyOs_resumeTask)
                : applyToHumiditySensorsTasks(MyOs_suspendTask);
            isTaskGroupSuspended[0] = !isTaskGroupSuspended[0];
        }
        if (taskSuspentionEvent.flags & 0b010) {
            isTaskGroupSuspended[1]
                ? applyToTemperatureSensorsTasks(MyOs_resumeTask)
                : applyToTemperatureSensorsTasks(MyOs_suspendTask);
            isTaskGroupSuspended[1] = !isTaskGroupSuspended[1];
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
    for (;;) {
        data.sensorData = 20 + (rand() % 5);
        MyOs_queueSend(&sensorDataQueue, &data);
        MyOs_taskDelay(_sensorCfg->periodMs);
    }
}

/* ************************************************************************* */
/*                              Main Definition                              */
/* ************************************************************************* */

int main(void) {
    uint8_t ids = 0;

    initHardware();
    MyOs_initialize();
    MyOs_eventCreate(&taskSuspentionEvent);
    MyOS_taskCreate(tasksSuspenderTask, NULL, 3, NULL);
    MyOS_taskCreate(uartSenderTask, NULL, 3, NULL);
    MyOS_taskCreate(sensorDataSerializerTask, NULL, 3, NULL);

    for (uint8_t i = 0; i < HUM_SENSORS_N; i++) {
        ids++;
        hum[i].greenhouseId = ids;
        hum[i].sensorId = ids;
        hum[i].periodMs = 300 * (rand() % 10);
        hum[i].type = SENSOR_TYPE_HUM;
        MyOS_taskCreate(sensorTask, &hum[i], 3, &humidityTaskHandles[i]);
    }
    for (uint8_t i = 0; i < TEMP_SENSORS_N; i++) {
        ids++;
        temp[i].greenhouseId = ids;
        temp[i].sensorId = ids;
        temp[i].periodMs = 100 * (rand() % 10);
        temp[i].type = SENSOR_TYPE_TMP;
        MyOS_taskCreate(sensorTask, &temp[i], 3, &temperatureTaskHandles[i]);
    }

    MyOs_installIRQ(PIN_INT0_IRQn, tec1DownIsr);
    MyOs_installIRQ(PIN_INT2_IRQn, tec2DownIsr);

    for (;;)
        ;
}
