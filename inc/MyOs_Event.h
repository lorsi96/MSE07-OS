#ifndef __MY_OS_EVENT__
#define __MY_OS_EVENT__

#include <stdint.h>

#include "MyOs_Types.h"
#include "MyOs_Core.h"

/* ************************************************************************* */
/*                              Public Functions                             */
/* ************************************************************************* */

void MyOs_createEvent(MyOs_Event_t* ev);

void MyOs_eventSet(MyOs_Event_t* ev, uint8_t flags);

void MyOs_eventPost(MyOs_Event_t* ev, uint8_t flags);

void MyOs_eventWait(MyOs_Event_t* ev, uint8_t flags);

void MyOs_eventWaitAll(MyOs_Event_t* ev, uint8_t flags);

#endif  // __MY_OS_EVENT__
