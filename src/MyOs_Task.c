#include "MyOs_Task.h"

#include <stdlib.h>

#include "MyOs_Hooks.h"

/* ************************************************************************* */
/*                       Private Functions Declarations                      */
/* ************************************************************************* */

/* *************************** Memory Management *************************** */
MyOs_TaskHandle_t __MyOs_taskNew();
void __MyOs_taskDelete(MyOs_TaskHandle_t task);

/* ************************** Return Hook Wrapper ************************** */
static void __MyOs_returnHookWrapper();

/* ************************** Tasks Initialization ************************* */
static void __MyOs_initTaskStack(MyOs_TCB_t* tcb, const void* taskCode,
                                 const void* taskParams);
static void __MyOs_initIdleTask();

/* ************************************************************************* */
/*                        Public Functions Definitions                       */
/* ************************************************************************* */
void MyOS_taskCreate(const void* taskCode, void* parameters, uint8_t priority,
                     MyOs_TaskHandle_t* handle) {
    MyOs_t* self = MyOs_getInstance();

    if ((self->numberOfTasks + 1) >= MAX_TASKS_N) {
        MyOs_raiseError(MyOS_taskCreate, MY_OS_ERROR_OUT_OF_MEMORY);
    }

    self->tasks[self->numberOfTasks] = __MyOs_taskNew();
    MyOs_TCB_t* tcb = self->tasks[self->numberOfTasks];
    tcb->id = self->numberOfTasks++;
    tcb->priority = priority + 1;
    if (tcb->priority > self->mxPrio) {
        self->mxPrio = tcb->priority;
    }

    __MyOs_initTaskStack(tcb, taskCode, parameters);
    if (handle != NULL) {
        *handle = tcb;
    }
}

MyOs_TaskHandle_t MyOs_getCurrentTask() {
    MyOs_t* self = MyOs_getInstance();
    return self->tasks[self->currentTaskId];
}

void MyOs_blockTask(MyOs_TaskHandle_t taskHandle) {
    MyOs_t* self = MyOs_getInstance();
    uint8_t taskId;

    if (taskHandle == NULL) {
        taskId = self->currentTaskId;
    } else {
        taskId = taskHandle->id;
        if (self->tasks[taskId]->priority == 0) {
            MyOs_raiseError(MyOs_blockTask, MY_OS_ERROR_TASK_ID_BLOCKED);
        }
    }

    if (taskId < self->numberOfTasks) {
        self->tasks[taskId]->state = MY_OS_TASK_STATE_BLOCKED;
        if (taskId == self->currentTaskId) {
            MyOs_yield();
        }
    }
}

void MyOs_unblockTask(MyOs_TaskHandle_t taskHandle) {
    MyOs_t* self = MyOs_getInstance();
    if (taskHandle->id < self->numberOfTasks) {
        if (taskHandle->id != self->currentTaskId) {
            self->tasks[taskHandle->id]->state = MY_OS_TASK_STATE_READY;
        }
    }
}

void MyOs_taskDelay(const uint32_t ticks) {
    MyOs_TaskHandle_t currentTask = MyOs_getCurrentTask();
    currentTask->delayCount = ticks;
    currentTask->state = MY_OS_TASK_STATE_BLOCKED;
    MyOs_yield();
    if (currentTask->delayCount > 0) {
        MyOs_raiseError(MyOs_taskDelay,
                        MY_OS_ERROR_TASK_CALLED_BEFORE_DELAY_FINISHED);
    }
}

/* ************************************************************************* */
/*                       Private Functions Definitions                       */
/* ************************************************************************* */

/* *************************** Memory Management *************************** */
MyOs_TaskHandle_t __MyOs_taskNew() {
#ifdef MY_OS_USE_DYNAMIC_MEMORY
    MyOs_TaskHandle_t task;
    if (!(task = calloc(sizeof(MyOs_TCB_t), 1))) {
        MyOs_mallocHook(MyOS_taskCreate);
    }
    return task;
#else
    static MyOs_TCB_t __tasks[MAX_TASKS_N] = {0};
    static uint8_t __tasks_index = 0;
    if (++__tasks_index > MAX_TASKS_N) {
        MyOs_mallocHook(MyOS_taskCreate);
    }
    return &__tasks[__tasks_index];
#endif
}

void __MyOs_taskDelete(MyOs_TaskHandle_t task) {
    MyOs_raiseError(__MyOs_taskDelete, MY_OS_ERROR_DELETE_NOT_IMPLEMENTED);
}

/* ************************** Return Hook Wrapper ************************** */
static void __MyOs_returnHookWrapper() {
    MyOs_t* self = MyOs_getInstance();
    MyOs_returnHook(self->tasks[self->currentTaskId]);
}

/* ************************** Tasks Initialization ************************* */
static void __MyOs_initTaskStack(MyOs_TCB_t* tcb, const void* taskCode,
                                 const void* taskParams) {
    tcb->stack.xpsr = INIT_XPSR;
    tcb->stack.lrPrev = EXEC_RETURN;
    tcb->stack.pc = (uint32_t)taskCode;
    tcb->stack.lr = (uint32_t)__MyOs_returnHookWrapper;
    tcb->stack.r0 = (uint32_t)taskParams;
    tcb->stack_pointer = (uint32_t)&tcb->stack.r11;
}

static void __MyOs_initIdleTask() {
    MyOS_taskCreate(MyOs_idleTask, NULL, 0xFF, NULL);
}