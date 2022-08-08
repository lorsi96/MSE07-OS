#ifndef MY_OS_ISR
#define MY_OS_ISR

/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include "MyOs_Kernel.h"
#include "board.h"
#include "cmsis_43xx.h"


/** @addtogroup MyOs_ISR
 * @{
 */

/* ************************************************************************* */
/*                                   Types                                   */
/* ************************************************************************* */
typedef void(*MyOs_IsrFunction)(void);

/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */
bool MyOs_installIRQ(LPC43XX_IRQn_Type irq, MyOs_IsrFunction isr_fun);

bool MyOs_uninstallIRQ(LPC43XX_IRQn_Type irq);

/* ************************************************************************* */

/** 
 * @}
 */

#endif