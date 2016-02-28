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

/**
 * @file
 * @brief Module for handling timed events, built upon Timer objects.
 * @author Bendegúz Nagy
 *
 * This module maintains an ordered list of timed events, which consists of
 * Timer_callBack function pointers, their state pointers. Also each one is assigned
 * a Timer object.
 *
 * Initialize and deinitialize the module with TM_init() and TM_deinit() respectively.
 * Register Timed_events with TM_new(), free them with TM_freeTimed_event().
 * Note: A Timed_event is destroyed automatically, if during a callback it returns a
 * non-zero constant.
 * Call TM_process() with the elapsed time in ms, which calls each registered Timed_event
 * with the elapsed time.
 * Destroy every Timed_event with TM_clear().
 *
 */


#ifndef DUMMY_TIMER_MAN_H
#define DUMMY_TIMER_MAN_H

#include "../HEAD/Timer_man.h"
#include "timer.h"

/**
 * @brief Timed_event function pointers have to adhere to this signature.
 * @param delta Elapsed time.
 * @param timer The event's own timer.
 * @param state The state pointer set during subscription.
 * @return non-zero if the Timed_event should be deleted.
 */
typedef int (*Timer_callBack)(Uint32 delta, Timer *timer, void *state);


typedef struct Timed_event {
    int id; //used for direct access from the array, do not modify
    Timer *timer;
    Timer_callBack callBack;
    void *state;
} Timed_event;

void TM_init();
void TM_deinit();


Timed_event *TM_new(Timer_callBack callBack, void *state);
void TM_freeTimed_event(Timed_event *e);


void TM_process(Uint32 delta);
void TM_clear();

#endif //DUMMY_TIMER_MAN_H
