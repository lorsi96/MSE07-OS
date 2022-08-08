/**
 * @file MyOs_ISR.h
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2022-08-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */
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
typedef void (*MyOs_IsrFunction)(void);

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