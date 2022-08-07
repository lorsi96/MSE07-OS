/**
 * @file MSE_OS_Kernel.c
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-16
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "MyOs_Kernel.h"

#include <stdlib.h>

#include "MyOs_Hooks.h"
#include "MyOs_Task.h"
#include "MyOs_ISR.h"
#include "cmsis_43xx.h"
#include "board.h"

/* ************************************************************************* */
/*                             Private Functions                             */
/* ************************************************************************* */
void __MyOs_spinDelayTicks() {
    MyOs_t* self = MyOs_getInstance();
    for (uint8_t taskId = 0; taskId < self->numberOfTasks; taskId++) {
        MyOs_TaskHandle_t t = self->tasks[taskId];
        if (t->state == MY_OS_TASK_STATE_BLOCKED && t->delayCount > 0) {
            if (--t->delayCount == 0) {
                t->state = MY_OS_TASK_STATE_READY;
            }
        }
    }
}

static void __MyOs_scheduler() {
    MyOs_t* self = MyOs_getInstance();

    if (self->state == MY_OS_GENERAL_STATE_RESET) {
        if (self->numberOfTasks == 0) {
            MyOs_errorHook(__MyOs_scheduler, MY_OS_ERROR_NO_TASKS);
        }
        self->currentTaskId = 0;
        self->contextSwitchRequested = true;
        return;
    }

    uint8_t candidateTaskId = (self->currentTaskId + 1) % self->numberOfTasks;
    self->contextSwitchRequested = false;
    for (uint8_t p = self->mxPrio; p >= 0; p--) {
        uint8_t firstPTaskId = 0xFF;
        uint8_t nTasks = 0;
        do { // TODO: Implement a better scheduler (but this one works!). 
            if (self->tasks[candidateTaskId]->priority == p) {
                if (firstPTaskId == 0xFF) firstPTaskId = candidateTaskId;
                switch (self->tasks[candidateTaskId]->state) {
                    case MY_OS_TASK_STATE_READY:
                        self->nextTaskId = candidateTaskId;
                        self->contextSwitchRequested = true;
                        return;  // Next task found.
                    case MY_OS_TASK_STATE_BLOCKED:
                        break;
                    case MY_OS_TASK_STATE_RUNNING:
                        self->nextTaskId = candidateTaskId;
                        self->contextSwitchRequested = true;
                        return;
                    default:
                        MyOs_raiseError(__MyOs_scheduler,
                                        MY_OS_ERROR_INVALID_TASK_STATE);
                }
            }
            candidateTaskId = (candidateTaskId + 1) % self->numberOfTasks;
            nTasks++;
        } while (candidateTaskId != firstPTaskId &&
                 (nTasks < self->numberOfTasks));
    }
}

static inline void __MyOs_requestPendSv() {
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    __ISB();
    __DSB();
}

static inline void __MyOs_configurePendSv() {
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

static void __MyOs_initIdleTask() {
    MyOS_taskCreate(MyOs_idleTask, NULL, 0xFF, NULL);
}

static inline void __MyOs_incrementCriticalCounter() {
    MyOs_t* self = MyOs_getInstance();
    self->criticalCounter++;
}

static inline void __MyOs_decrementCriticalCounter() {
    MyOs_t* self = MyOs_getInstance();
    self->criticalCounter--;
    if(self->criticalCounter < 0) {
       self->criticalCounter = 0; 
    }
}


/* ************************************************************************* */
/*                                ISR Handlers                               */
/* ************************************************************************* */
/**
 * @brief ISR Handler. Enables PendSV Interruptions (lowest priority).
 *
 */
void SysTick_Handler(void) {  // Overrides weak handler.
    MyOs_t* self = MyOs_getInstance();

    __MyOs_scheduler();
    __MyOs_spinDelayTicks();
    MyOs_tickHook();
    if (self->contextSwitchRequested) {
        __MyOs_requestPendSv();
    }
}

/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */

MyOs_t* MyOs_getInstance() {
    static MyOs_t instance = {.error = MY_OS_ERROR_NONE,
                              .numberOfTasks = 0,
                              .currentTaskId = -1,
                              .nextTaskId = -1,
                              .state = MY_OS_GENERAL_STATE_RESET};
    return &instance;
}

void MyOs_raiseError(void* caller, MyOs_Error_t err) {
    MyOs_t* self = MyOs_getInstance();
    self->error = err;
    MyOs_errorHook(caller, err);
}

void MyOs_initialize(void) {
    __MyOs_initIdleTask();
    __MyOs_configurePendSv();
}

void MyOs_yield() {
    MyOs_t* self = MyOs_getInstance();
    __MyOs_scheduler();
    if (self->contextSwitchRequested) {
        __MyOs_requestPendSv();
    }
}

/**
 * @brief Enters critical section.
 * 
 */
void MyOs_enterCritical() {
    __disable_irq();
    __MyOs_incrementCriticalCounter();
}

/**
 * @brief Exits critical section.
 * 
 */
void MyOs_exitCritical() {
    __MyOs_decrementCriticalCounter();
    __enable_irq();
}

void MyOs_updateState(MyOs_GeneralState_t state) {
    MyOs_t* self = MyOs_getInstance();
    self->state = state;
}

bool MyOs_isContextISR() {
    return MyOs_getInstance()->state == MY_OS_GENERAL_STATE_IRQ_RUNNING;
}

/* ****************** Pend SV Interrupt: Context Switcher ****************** */
uint32_t __MyOs_getNextContext(uint32_t currentSp) {
    MyOs_t* self = MyOs_getInstance();
    if (self->state == MY_OS_GENERAL_STATE_RESET) {
        self->state = MY_OS_GENERAL_STATE_RUN;
        self->tasks[self->currentTaskId]->state = MY_OS_TASK_STATE_RUNNING;
    } else {
        self->tasks[self->currentTaskId]->stack_pointer = currentSp;
        if (self->tasks[self->currentTaskId]->state ==
            MY_OS_TASK_STATE_RUNNING) {
            self->tasks[self->currentTaskId]->state = MY_OS_TASK_STATE_READY;
        }
        self->currentTaskId = self->nextTaskId;
        self->tasks[self->currentTaskId]->state = MY_OS_TASK_STATE_RUNNING;
    }
    return self->tasks[self->currentTaskId]->stack_pointer;
}
/* ************************************************************************* */
