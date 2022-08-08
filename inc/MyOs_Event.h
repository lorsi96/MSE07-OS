/**
 * @file MyOs_Event.h
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-31
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __MY_OS_EVENT__
#define __MY_OS_EVENT__

/** @addtogroup MyOs_Event
 * @{
 */

/* ************************************************************************* */
/*                                 Inclusions                                */
/* ************************************************************************* */
#include <stdint.h>

#include "MyOs_Kernel.h"
#include "MyOs_Task.h"
#include "MyOs_Types.h"

/* ************************************************************************* */
/*                                Public Types                               */
/* ************************************************************************* */
/**
 * @brief Event flags synchronization primitive.
 *
 */
typedef struct {
    uint8_t flags;                  //<! flags representing the event.
    MyOs_TaskHandle_t waitingTask;  //<! task waiting for event.
} MyOs_Event_t;

/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */

/**
 * @brief Creates an event object using static memory.
 *
 * @param[inout] event event container to be initialized.
 */
void MyOs_eventCreate(MyOs_Event_t* event);

/**
 * @brief Overwrites the entire event flags.
 *
 * @details  Differs from eventPost in the sense that eventSet overwrites the
 *           flag values, whereas post performs an "or" operation with the
 *           incoming value and the current event state/flags.
 *
 * @param ev initialized event object.
 * @param flags new value of event flags to overwrite the previous one.
 */
void MyOs_eventSet(MyOs_Event_t* ev, uint8_t flags);

/**
 * @brief Posts any number of events in a single operation.
 *
 * @details  Differs from eventSet in the sense that eventSet overwrites the
 *           flag values, whereas post performs an "or" operation with the
 *           incoming value and the current event state/flags.
 *
 * @param ev initialized event object.
 * @param flags event flags to be "or-ed" to the current event flags.
 */
void MyOs_eventPost(MyOs_Event_t* ev, uint8_t flags);

/**
 * @brief Blocks task until any listed event occurs.
 *
 * @details The activation of ANY flag from the flags parameter will unblock
 *          the waiting task. @see MyOs_eventWaitAll for a different behaviour.
 *
 * @param ev initialized event object.
 * @param flags relevant event flags to wait for.
 */
void MyOs_eventWait(MyOs_Event_t* ev, uint8_t flags, uint32_t msToWait);

/**
 * @brief Blocks task until agoup of events occur.
 *
 * @details Only the activation of ALL of the flags in the flags parameter will
 *          unblock the waiting task. @see MyOs_eventWait for a different
 *          behaviour.
 *
 * @param ev initialized event object.
 * @param flags exact event to wait for.
 */
void MyOs_eventWaitAll(MyOs_Event_t* ev, uint8_t flags, uint32_t msToWait);

/* ************************************************************************* */

/**
 * @}
 */

#endif  // __MY_OS_EVENT__
