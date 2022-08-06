	.syntax unified
	.global PendSV_Handler
	.text
	.thumb_func

PendSV_Handler:

	cpsid i
	tst lr,0x10
	it eq
	vpusheq {s16-s31}

	push {r4-r11,lr}
	mrs r0,msp
	bl __MyOs_getNextContext
	msr msp,r0
	pop {r4-r11,lr}

	tst lr,0x10
	it eq
	vpopeq {s16-s31}

	cpsie i

	bx lr				
