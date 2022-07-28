#include "MyOs_Types.h"
#include "MyOs_Hooks.h"


void __weak MyOs_returnHook() {
    for(;;);
}

void __weak MyOs_tickHook() {
    __asm volatile("nop");
}

void __weak MyOs_errorHook(void* caller, MyOs_Error_t err) {
    (void)caller;
    (void)err;
    for(;;);
}