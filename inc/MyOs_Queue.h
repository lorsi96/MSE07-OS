/**
 * @file MyOs_queue.h
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-31
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __MY_OS_QUEUE__
#define __MY_OS_QUEUE__

/** @addtogroup MyOs_Queue
 * @{
 */

/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "MyOs_Kernel.h"
#include "MyOs_Task.h"
#include "MyOs_Types.h"

/* ************************************************************************* */
/*                                Public Types                               */
/* ************************************************************************* */

/**
 * @brief Queue synchronization primitive.
 *
 */
typedef struct {
    uint32_t length;
    uint32_t itemSizeBytes;
    uint8_t* buffer;
    MyOs_TaskHandle_t waitingTask;  //<! task waiting for this queue.
    uint8_t head;
    uint8_t tail;
} MyOs_Queue_t;

/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */
/**
 * @brief Declares and creates a queue.
 * 
 * @param name the name of the queue variable to be declared.
 * @param dtype type of the elements that the queue will contain.
 * @param len queue length (MAX number of elements).
 * 
 */
#define MyOs_queue_CREATE_STATIC(name, dtype, len) \
  uint8_t name##buffer[(sizeof(dtype)) * ((len) + 1)];\
  MyOs_Queue_t name = {\
    .length=len+1,\
    .itemSizeBytes=sizeof(dtype),\
    .buffer=name##buffer,\
    .waitingTask=NULL,\
    .head=0,\
    .tail=0\
  };

/**
 * @brief Sends an item to the message queue.
 * 
 * @param[in] queue target queue.
 * @param[in] item referece to the item to be sent.
 */
void MyOs_queueSend(MyOs_Queue_t* queue, const void* item);

/**
 * @brief Receives an element from a queue
 * 
 * @param[in] queue target queue.
 * @param[out] item received from the queue. 
 */
void MyOs_queueReceive(MyOs_Queue_t* queue, void* item);

/* ************************************************************************* */

/**
 * @}
 */
#endif