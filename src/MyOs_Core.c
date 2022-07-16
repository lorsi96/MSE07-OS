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
#include "board.h"

/* ************************************************************************* */
/*                             Private Functions                             */
/* ************************************************************************* */
static MyOs_t* __MyOs_getInstance() {
    static MyOs_t instance = {
		.error = MY_OS_ERROR_NONE,
		.numberOfTasks = 0,
		.currentTaskId = -1,
		.nextTaskId = -1,
		.state = MY_OS_GENERAL_STATE_RESET
	};
    return &instance;
}

/* ************************************************************************* */
/*                                ISR Handlers                               */
/* ************************************************************************* */
/**
 * @brief ISR Handler. Enables PendSV Interruptions (lowest priority).
 * 
 */
void SysTick_Handler(void) { // Overrides weak handler.
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	__ISB();
	__DSB();
}

/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */

void MyOs_initialize(void) {	
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);
}


MyOs_Error_t MyOS_taskCreate(const void *taskCode, MyOs_TaskHandle_t* handle) {
	MyOs_t* self = __MyOs_getInstance();
	if ((self->numberOfTasks + 1) > MAX_TASKS_N) {
		return MY_OS_ERROR_OUT_OF_MEMORY;
	}
	MyOs_TCB_t* tmb = &self->tasks[self->numberOfTasks++];
	tmb->stack.xpsr = INIT_XPSR;
	tmb->stack.lrPrev = EXEC_RETURN;
	tmb->stack.pc = (uint32_t)taskCode;
	tmb->stack_pointer = (uint32_t)&tmb->stack.r11;
	if(handle != NULL) {
		*handle = tmb;
	}
	return MY_OS_ERROR_NONE;
}


uint32_t MyOs_getNextContext(uint32_t currentSp) {
	MyOs_t* self = __MyOs_getInstance();
	if(self->currentTaskId > -1) {
		self->tasks[self->currentTaskId].stack_pointer = currentSp;
	}
	self->currentTaskId = (self->currentTaskId + 1) % self->numberOfTasks;
	return self->tasks[self->currentTaskId].stack_pointer;
}


