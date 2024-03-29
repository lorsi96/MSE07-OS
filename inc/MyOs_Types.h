/**
 * @file MyOs_Types.h
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-16
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __MY_OS_TYPES__
#define __MY_OS_TYPES__

/* ************************************************************************* */
/*                                  Includes                                 */
/* ************************************************************************* */
#include <stdbool.h>
#include <stdint.h>

#include "MysOs_Config.h"

/* ************************************************************************* */
/*                           Preprocessor Constants                          */
/* ************************************************************************* */
#define STACK_FRAME_SIZE 8
#define FULL_REG_STACKING_SIZE (STACK_FRAME_SIZE + 8 + 1)

#define MY_OS_MAX_DELAY 0
/* ************************************************************************* */
/*                                   Types                                   */
/* ************************************************************************* */

/**
 * @brief Tasks stack layout.
 */
typedef struct {
    uint32_t __stack[STACK_SIZE - FULL_REG_STACKING_SIZE];
    uint32_t r11;
    uint32_t r10;
    uint32_t r9;
    uint32_t r8;
    uint32_t r7;
    uint32_t r6;
    uint32_t r5;
    uint32_t r4;
    uint32_t lrPrev;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
} MyOs_TaskStackLayout_t;

/**
 * @brief Task states.
 *
 */
typedef enum {
    MY_OS_TASK_STATE_READY = 0,
    MY_OS_TASK_STATE_RUNNING = 1,
    MY_OS_TASK_STATE_BLOCKED = 2,
    MY_OS_TASK_STATE_SUSPENDED = 3,
} MyOs_TaskState_t;

/**
 * @brief Task Control Block structure.
 *
 */
typedef struct {
    MyOs_TaskStackLayout_t stack;
    uint32_t stack_pointer;
    uint8_t id;
    MyOs_TaskState_t state;
    MyOs_TaskState_t state_before_suspension;
    uint8_t priority;
    uint32_t delayCount;
} MyOs_TCB_t;

/**
 * @brief OS engine general state.
 *
 */
typedef enum {
    MY_OS_GENERAL_STATE_RUN = 0,
    MY_OS_GENERAL_STATE_RESET = 1,
    MY_OS_GENERAL_STATE_SCHEDULING = 2,
    MY_OS_GENERAL_STATE_IRQ_RUNNING = 3,
} MyOs_GeneralState_t;

/**
 * @brief Error types.
 *
 */
typedef enum {
    MY_OS_ERROR_NONE = 0,
    MY_OS_ERROR_OUT_OF_MEMORY = 1,
    MY_OS_ERROR_INVALID_TASK_STATE = 2,
    MY_OS_ERROR_NO_TASKS = 3,
    MY_OS_ERROR_TASK_ID_BLOCKED = 4,
    MY_OS_ERROR_EVENT_ALREADY_AWAITED = 5,
    MY_OS_ERROR_TASK_CALLED_BEFORE_DELAY_FINISHED = 6,
    MY_OS_ERROR_DELETE_NOT_IMPLEMENTED = 7,
    MY_OS_ERROR_MEMORY_ALLOCATION_ERROR = 8,
    MY_OS_ERROR_ZERO_SIZE_QUEUE = 9,
    MY_OS_ERROR_ISR_INDEX_OUT_OF_RANGE = 10,
    MY_OS_ERROR_DELAY_FROM_ISR = 11
} MyOs_Error_t;

/**
 * @brief Opaque pointer to task control block.
 *
 */
typedef MyOs_TCB_t* MyOs_TaskHandle_t;

/**
 * @brief Operating system general control structure.
 *
 */
typedef struct {
    MyOs_TaskHandle_t
        tasks[MAX_TASKS_N + 1];  //!< Static memory allocated for tasks.
    uint8_t numberOfTasks;       //!< Current number of tasks being used.
    int8_t currentTaskId;        //!< Id of currently running task.
    int8_t nextTaskId;           //!< Id of task to be run afterwards.
    MyOs_Error_t error;          //!< Cached error. See #MyOs_Error_t.
    MyOs_GeneralState_t state;
    bool contextSwitchRequested;
    uint8_t mxPrio;
    int8_t criticalCounter;
    bool isrSchedulingRequested;
} MyOs_t;
/* ************************************************************************* */

#endif  // __MY_OS_TYPES__