/**
 * @file MyOs_semaphore.h
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-31
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __MY_OS_SEMAPHORE__
#define __MY_OS_SEMAPHORE__

/** @addtogroup MyOs_Semaphore
 * @{
 */

/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "MyOs_Kernel.h"
#include "MyOs_Queue.h"
#include "MyOs_Task.h"
#include "MyOs_Types.h"

/* ************************************************************************* */
/*                                Public Types                               */
/* ************************************************************************* */

/**
 * @brief Semaphore synchronization primitive.
 *
 */
typedef MyOs_Queue_t MyOs_Semaphore_t;

/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */

/**
 * @brief Declares and creates a semaphore object.
 *
 * @param name name of the semaphore variable.
 */
#define MyOs_semaphore_CREATE_STATIC(name) \
    MyOs_queue_CREATE_STATIC(name, uint8_t, 1);

/**
 * @brief Gives the semaphore.
 *
 * @param semaphore
 */
void MyOs_semaphoreGive(MyOs_Semaphore_t* semaphore) {
    static uint8_t _;
    MyOs_queueSend(semaphore, &_);
}

/**
 * @brief Takes the semaphore.
 *
 * @param semaphore
 */
void MyOs_semaphoreTake(MyOs_Semaphore_t* semaphore, uint32_t msToWait) {
    static uint8_t _;
    MyOs_queueReceive(semaphore, &_, msToWait);
}

/* ************************************************************************* */

/**
 * @}
 */
#endif