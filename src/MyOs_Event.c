/**
 * @file MyOs_Event.c
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-31
 *
 * @copyright Copyright (c) 2022
 *
 */

/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include "MyOs_Event.h"

#include <stdlib.h>

#include "MyOs_Kernel.h"

/* ************************************************************************* */
/*                        Public Functions Definitions                       */
/* ************************************************************************* */

void MyOs_eventCreate(MyOs_Event_t* ev) {
    ev->flags = 0x00;
    ev->waitingTask = NULL;
}

void MyOs_eventSet(MyOs_Event_t* ev, uint8_t flags) {
    ev->flags = flags;
    if (ev->waitingTask) {
        MyOs_unblockTask(ev->waitingTask);
    }
}

void MyOs_eventPost(MyOs_Event_t* ev, uint8_t flags) {
    ev->flags |= flags;
    if (ev->waitingTask) {
        MyOs_unblockTask(ev->waitingTask);
    }
}

void MyOs_eventWait(MyOs_Event_t* ev, uint8_t flags, uint32_t msToWait) {
    if (!ev->waitingTask) {
        ev->waitingTask = MyOs_getCurrentTask();
    } else {
        MyOs_raiseError(MyOs_eventWait, MY_OS_ERROR_EVENT_ALREADY_AWAITED);
    }
    while (!(ev->flags & flags)) {
        MyOs_blockTask(NULL, msToWait);
    }
    ev->waitingTask = NULL;
}

void MyOs_eventWaitAll(MyOs_Event_t* ev, uint8_t flags, uint32_t msToWait) {
    if (!ev->waitingTask) {
        ev->waitingTask = MyOs_getCurrentTask();
    } else {
        MyOs_raiseError(MyOs_eventWait, MY_OS_ERROR_EVENT_ALREADY_AWAITED);
    }
    while (!((ev->flags & flags) == flags)) {
        MyOs_blockTask(NULL, msToWait);
    }
}

/* ********************************** EOF ********************************** */