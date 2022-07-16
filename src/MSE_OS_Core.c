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
#include "MSE_OS_Core.h"
#include "board.h"

/* ************************************************************************* */
/*                                Private Data                               */
/* ************************************************************************* */
static TaskMemoryBlock_t __tasksMemBlocks[MAX_TASKS_N];
static uint8_t __tasksMemBlkCount = 0;
static int8_t __currentTask = -1;


void MyOs_init(void) {
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);
}


MyOs_TaskHandle_t MyOS_taskCreate(void *taskCode) {
	if ((__tasksMemBlkCount + 1) > MAX_TASKS_N) {
		return NULL;
	}
	TaskMemoryBlock_t* tmb = &__tasksMemBlocks[__tasksMemBlkCount++];
	tmb->stack.xpsr = INIT_XPSR;
	tmb->stack.lr_prev = EXEC_RETURN;
	tmb->stack.pc = (uint32_t)taskCode;
	tmb->stack_pointer = (uint32_t)&tmb->stack.r11;
	return tmb;
}


uint32_t MyOs_getNextContext(uint32_t currentSp) {
	if(__currentTask > -1) {
		__tasksMemBlocks[__currentTask].stack_pointer = currentSp;
	}
	__currentTask = (++__currentTask) % __tasksMemBlkCount;
	return __tasksMemBlocks[__currentTask].stack_pointer;
}

/**
 * @brief ISR Handler. Enables PendSV Interruptions (lowest priority).
 * 
 */
void SysTick_Handler(void) { // Overrides weak handler.
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	__ISB();
	__DSB();
}
