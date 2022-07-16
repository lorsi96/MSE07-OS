#ifndef ISO_I_2020_MSE_OS_INC_MSE_OS_CORE_H_
#define ISO_I_2020_MSE_OS_INC_MSE_OS_CORE_H_

#include <stdint.h>

#define STACK_SIZE                  64
#define MAX_TASKS_N                 8
#define STACK_FRAME_SIZE	        8
#define FULL_REG_STACKING_SIZE      (STACK_FRAME_SIZE + 8)
#define INIT_XPSR 	                1 << 24				
#define EXEC_RETURN	                0xFFFFFFF9

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
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
} Stack_t;

typedef struct {
	Stack_t stack;
	uint32_t stack_pointer;
} TaskMemoryBlock_t;

void MyOS_taskCreate(void *taskCode);


#endif /* ISO_I_2020_MSE_OS_INC_MSE_OS_CORE_H_ */
