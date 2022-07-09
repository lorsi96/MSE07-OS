/*==================[inclusions]=============================================*/

#include "main.h"

#include "board.h"
#include "sapi.h"
#include "MSE_OS_Core.h"


/*==================[macros and definitions]=================================*/

#define MILISEC		1000

/*==================[Global data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void)  {
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / MILISEC);		//systick 1ms
}


/*==================[Definicion de tareas para el OS]==========================*/
void tarea1(void)  {
	int i;
	while (1) {
		i++;
	}
}

void tarea2(void)  {
	int j;
	while (1) {
		j++;
	}
}


/*============================================================================*/

int main(void)  {

	initHardware();

	MyOS_taskCreate(tarea1);
	MyOS_taskCreate(tarea2);

	while (1) {
		__WFI();
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
