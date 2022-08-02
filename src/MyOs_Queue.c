/**
 * @file MyOs_Queue.c
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2022-07-31
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "MyOs_Queue.h"

/* ************************************************************************* */
/*                             Private Functions                             */
/* ************************************************************************* */
static inline void __MyOs_queuePush(MyOs_Queue_t* queue, const void* item) {
    memcpy(queue->buffer + (queue->itemSizeBytes * queue->head), item, queue->itemSizeBytes);
    queue->head = (queue->head + 1) % queue->length; 
}

static inline void __MyOs_queuePop(MyOs_Queue_t* queue, void* item) {
    memcpy(item, queue->buffer + (queue->itemSizeBytes * queue->tail), queue->itemSizeBytes);
    queue->tail = (queue->tail + 1) % queue->length; 
}



/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */

void MyOs_queueSend(MyOs_Queue_t* queue, const void* item) {

    /* If queue is full, wait until some other task consumes an element.*/
    while((queue->head + 1) % queue->length == queue->tail) { 
        queue->waitingTask = MyOs_getCurrentTask();
        MyOs_blockTask(NULL);
    }

    __MyOs_queuePush(queue, item);
    
    /* Unblock any task that might be waiting for data in this queue. */
    if(queue->waitingTask) {
        MyOs_unblockTask(queue->waitingTask);
    }    
}


void MyOs_queueReceive(MyOs_Queue_t* queue, void* item) {
    
    /* If queue is empty, wait until some other task consumes an element.*/
    while(queue->head == queue->tail) { 
        queue->waitingTask = MyOs_getCurrentTask();
        MyOs_blockTask(NULL);
    }

    __MyOs_queuePop(queue, item);

    /* Unblock any task that might be attempting to push to this full queue. */
    if(queue->waitingTask) {
        MyOs_unblockTask(queue->waitingTask);
    }    
}

/* ************************************************************************* */