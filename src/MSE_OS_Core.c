/*
 * MSE_OS_Core.c
 *
 *  Created on: 26 mar. 2020
 *      Author: gonza
 */

#include "MSE_OS_Core.h"


/* ************************************************************************* */
/*                                Private Data                               */
/* ************************************************************************* */
static TaskMemoryBlock_t __tasksStack[MAX_TASKS_N];
static uint8_t __tasksStackCount = 0;


uint32_t sp_tarea1; 
uint32_t sp_tarea2;


void MyOS_taskCreate(void *taskCode) {
	TaskMemoryBlock_t* tmb = &__tasksStack[__tasksStackCount++];
	tmb->stack.xpsr = INIT_XPSR;
	tmb->stack.pc = taskCode;
	tmb->stack_pointer = (uint32_t)&tmb->stack.r0;
	sp_tarea1 = __tasksStack[0].stack_pointer;
	sp_tarea2 = __tasksStack[1].stack_pointer;
}
