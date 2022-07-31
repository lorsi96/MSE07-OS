#include "MyOs_Types.h"
#include "MyOs_Hooks.h"


__weak void MyOs_returnHook(MyOs_TaskHandle_t returningTask) {
    for(;;) {
        __asm("wfi");
    }
}

__weak void MyOs_tickHook() {
    __asm volatile("nop");
}

__weak void MyOs_errorHook(void* caller, MyOs_Error_t err) {
    (void)caller;
    (void)err;
    for(;;);
}

__weak void MyOs_mallocHook(void* caller) {
    MyOs_errorHook(caller, MY_OS_ERROR_MEMORY_ALLOCATION_ERROR);
}

__weak void MyOs_idleTask() {
    for(;;) {
        __asm("wfi");
    }
}