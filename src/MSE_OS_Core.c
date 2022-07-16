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
	tmb->stack.pc = (uint32_t)taskCode;
	tmb->stack_pointer = (uint32_t)&tmb->stack.r11;
	sp_tarea1 = __tasksStack[0].stack_pointer;
	sp_tarea2 = __tasksStack[1].stack_pointer;
}
