	.syntax unified
	.global PendSV_Handler
	.text
	.thumb_func

PendSV_Handler:
	push {r4-r11,lr}
	mrs r0,msp
	bl MyOs_getNextContext
	msr msp,r0
	pop {r4-r11,lr}
	bx lr
