/**
 * @file MSE_OS_Core.c
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-16
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "MyOs_Core.h"
#include "MyOs_Hooks.h"
#include "MyOs_Event.h"

#include "board.h"
#define IDLE_TASK_ID  MAX_TASKS_N

/* ************************************************************************* */
/*                             Private Functions                             */
/* ************************************************************************* */
static MyOs_t* __MyOs_getInstance() {
    static MyOs_t instance = {.error = MY_OS_ERROR_NONE,
                              .numberOfTasks = 0,
                              .currentTaskId = -1,
                              .nextTaskId = -1,
                              .state = MY_OS_GENERAL_STATE_RESET};
    return &instance;
}

static inline void __MyOs_raiseError(void* caller, MyOs_Error_t err) {
    MyOs_t* self = __MyOs_getInstance();
    self->error = err;
    MyOs_errorHook(caller, err);
}


static void __MyOs_scheduler() {
    MyOs_t* self = __MyOs_getInstance();
    
    if (self->state == MY_OS_GENERAL_STATE_RESET) {
        if(self->numberOfTasks == 0) {
            MyOs_errorHook(__MyOs_scheduler, MY_OS_ERROR_NO_TASKS);
        }
        self->currentTaskId = 0;
        self->contextSwitchRequested = true;
        return;
    }

    volatile uint8_t firstTargetTaskId = (self->currentTaskId + 1) % self->numberOfTasks;
    volatile uint8_t candidateTaskId = firstTargetTaskId;
    self->contextSwitchRequested = false;
    do {
        switch (self->tasks[candidateTaskId].state) {
        case MY_OS_TASK_STATE_READY:
            self->nextTaskId = candidateTaskId;
            self->contextSwitchRequested = true;
            return; // Next task found.
        case MY_OS_TASK_STATE_BLOCKED: 
        case MY_OS_TASK_STATE_RUNNING: 
            break;
        default:
            __MyOs_raiseError(__MyOs_scheduler, MY_OS_ERROR_INVALID_TASK_STATE);
        }
        candidateTaskId = (candidateTaskId + 1) % self->numberOfTasks;
    } while(candidateTaskId != firstTargetTaskId);
    
    // No task is ready, run Idle task.
    self->nextTaskId = IDLE_TASK_ID;
    self->contextSwitchRequested = true;
}

static inline void __MyOs_requestPendSv() {
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    __ISB();
    __DSB();
}

static inline void __MyOs_configurePendSv() {
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

/* ************************************************************************* */
/*                                ISR Handlers                               */
/* ************************************************************************* */
/**
 * @brief ISR Handler. Enables PendSV Interruptions (lowest priority).
 *
 */
void SysTick_Handler(void) {  // Overrides weak handler.
    MyOs_t* self = __MyOs_getInstance();
    
    __MyOs_scheduler();
    MyOs_tickHook();
    if(self->contextSwitchRequested) {
        __MyOs_requestPendSv();
    }
}

static void __MyOs_initTaskStack(MyOs_TCB_t* tcb, const void* taskCode, const void* taskParams) {
    tcb->stack.xpsr = INIT_XPSR;
    tcb->stack.lrPrev = EXEC_RETURN;
    tcb->stack.pc = (uint32_t)taskCode;
    tcb->stack.r0 = (uint32_t)taskParams;
    tcb->stack_pointer = (uint32_t)&tcb->stack.r11;
}

static void __MyOs_initIdleTask() {
    MyOs_t* self = __MyOs_getInstance();
    __MyOs_initTaskStack(&self->tasks[IDLE_TASK_ID], MyOs_idleTask, NULL);
}

static MyOs_TaskHandle_t __MyOs_getCurrentTask() {
    MyOs_t* self = __MyOs_getInstance();
    return &self->tasks[self->currentTaskId];
}

/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */

void MyOs_initialize(void) {
    __MyOs_initIdleTask();
    __MyOs_configurePendSv();    
}

void MyOS_taskCreate(const void* taskCode, void* parameters,
                             MyOs_TaskHandle_t* handle) {
    MyOs_t* self = __MyOs_getInstance();

    if ((self->numberOfTasks + 1) >= MAX_TASKS_N) {
        __MyOs_raiseError(MyOS_taskCreate, MY_OS_ERROR_OUT_OF_MEMORY);
    }

    MyOs_TCB_t* tcb = &self->tasks[self->numberOfTasks++];
    __MyOs_initTaskStack(tcb, taskCode, parameters);
    
    if (handle != NULL) {
        *handle = tcb;
    }
}


void MyOs_blockTask(MyOs_TaskHandle_t taskHandle) {
    MyOs_t* self = __MyOs_getInstance();
    uint8_t taskId;

    if(taskHandle == NULL) {
        taskId = self->currentTaskId;
    } else {
        taskId = taskHandle->id;
        if(taskId == IDLE_TASK_ID) {
            __MyOs_raiseError(MyOs_blockTask, MY_OS_ERROR_TASK_ID_BLOCKED);
        }
    }
    
    if(taskId < self->numberOfTasks) {
        self->tasks[taskId].state = MY_OS_TASK_STATE_BLOCKED;
        if(taskId == self->currentTaskId) {
            MyOs_taskYield();
        }
    }
    
}

void MyOs_unblockTask(MyOs_TaskHandle_t taskHandle) {
    MyOs_t* self = __MyOs_getInstance();
    if(taskHandle->id < self->numberOfTasks) {
        if(taskHandle->id != self->currentTaskId) {
            self->tasks[taskHandle->id].state = MY_OS_TASK_STATE_READY;
        }
    }
}

void MyOs_taskYield() {
    __MyOs_scheduler();
}


/* ****************** Pend SV Interrupt: Context Switcher ****************** */
uint32_t __MyOs_getNextContext(uint32_t currentSp) {
    MyOs_t* self = __MyOs_getInstance();
    if (self->state == MY_OS_GENERAL_STATE_RESET) {
        self->state = MY_OS_GENERAL_STATE_RUN;
        self->tasks[self->currentTaskId].state = MY_OS_TASK_STATE_RUNNING;
    } else {
        self->tasks[self->currentTaskId].stack_pointer = currentSp;
        if(self->tasks[self->currentTaskId].state == MY_OS_TASK_STATE_RUNNING) {
            self->tasks[self->currentTaskId].state = MY_OS_TASK_STATE_READY;
        }
        self->currentTaskId = self->nextTaskId;
        self->tasks[self->currentTaskId].state = MY_OS_TASK_STATE_RUNNING;
    }
    return self->tasks[self->currentTaskId].stack_pointer;
}



/* ************************************************************************* */
/*                                 Event API                                 */
/* ************************************************************************* */

void MyOs_createEvent(MyOs_Event_t* ev) {
    ev->flags = 0x00;
    ev->waitingTask = NULL;
}

void MyOs_eventSet(MyOs_Event_t* ev, uint8_t flags) {
    ev->flags = flags;
    if(ev->waitingTask) {
        MyOs_unblockTask(ev->waitingTask);
    }
}

void MyOs_eventPost(MyOs_Event_t* ev, uint8_t flags) {
    ev->flags |= flags;
    if(ev->waitingTask) {
        MyOs_unblockTask(ev->waitingTask);
    }
}

void MyOs_eventWait(MyOs_Event_t* ev, uint8_t flags) {
    if(!ev->waitingTask) {
        ev->waitingTask = __MyOs_getCurrentTask();
    } else {
        __MyOs_raiseError(MyOs_eventWait, MY_OS_ERROR_EVENT_ALREADY_AWAITED);
    }
    while(!(ev->flags & flags)) {
        MyOs_blockTask(NULL);
    }
}

void MyOs_eventWaitAll(MyOs_Event_t* ev, uint8_t flags) {
    if(!ev->waitingTask) {
        ev->waitingTask = __MyOs_getCurrentTask();
    } else {
        __MyOs_raiseError(MyOs_eventWait, MY_OS_ERROR_EVENT_ALREADY_AWAITED);
    }
    while(!(ev->flags ^ flags)) {
        MyOs_blockTask(NULL);
    }
}
