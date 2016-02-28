/*
* Copyright (C) 2015 Bendegúz Nagy
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../HEAD/Timer_man.h"
#include "../../Utility/HEAD/bag.h"

/**
 * @brief Internal list holding the Timed_events.
 */
static Bag *eventStack = NULL;

/**
 * @brief Create a new timed event and register it.
 * @param callBack The callback function.
 * @param state The state function which will be passed to the function with each function.
 * @return Returns the newly allocated Timed_event.
 */
Timed_event *TM_new(Timer_callBack callBack, void *state)
{
    Timed_event *e = (Timed_event *) malloc(sizeof(Timed_event));
    e->callBack = callBack;
    e->state = state;
    e->timer = Timer_new();
    Timer_start(e->timer);

    //set the index at which this timed event is stored
    e->id = eventStack->elemCount;
    Bag_push(e, eventStack);

    return e;
}

/**
 * @brief Deallocate a Timed_event, this can also be done automatically.
 * @param e The event to be destroyed.
 *
 * Deallocate a Timed_event, note that if during a TM_process() an event's callback
 * function returns non-zero, it will be automatically deleted.
 */
void TM_freeTimed_event(Timed_event *e)
{
    Bag_unorderedRemove(e->id, eventStack);
    if (eventStack->vector[e->id] != NULL)
        ((Timed_event *) (eventStack->vector[e->id]))->id = e->id;
    Timer_free(e->timer);
    free(e);
}

/**
 * @brief Initialize the module.
 *
 * Initialize the module, this has to be called before th module is put to use. Calling this
 * more than once without calling TM_deinit() in between will cause memory leak.
 */
void TM_init()
{
    eventStack = Bag_new((freeData) &TM_freeTimed_event);
}

/**
 * @brief Deinitialize the module.
 */
void TM_deinit()
{
    Bag_free(eventStack, 1);
}

/**
 * @brief Destroy each Timed_event.
 */
void TM_clear()
{
    TM_deinit();
    TM_init();
}

/**
 * @brief Process the Timed_events with the elapsed time passed in ms.
 * @param delta Elapsed time since the last call in ms.
 */
void TM_process(Uint32 delta)
{
    int i;
    Timed_event *e;
    for (i = 0; i < eventStack->elemCount; i++) {
        e = eventStack->vector[i];
        //if the return value is not 0, means the timer wants itself deleted
        if (e->callBack(delta, e->timer, e->state) != 0) {
            TM_freeTimed_event(e);
        }
    }
}
