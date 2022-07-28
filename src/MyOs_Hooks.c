#include "MyOs_Types.h"
#include "MyOs_Hooks.h"


__weak void MyOs_returnHook() {
    for(;;);
}

__weak void MyOs_tickHook() {
    __asm volatile("nop");
}

__weak void MyOs_errorHook(void* caller, MyOs_Error_t err) {
    (void)caller;
    (void)err;
    for(;;);
}

__weak void MyOs_idleTask() {
    for(;;) {
        __asm("wfi");
    }
}