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
 * @brief Module for object which track time.
 * @author Bendegúz Nagy
 *
 * This s a module for time-keeping objects. They can be started, then updated
 * with the number of ms passed. The implementation is minimal, because this project
 * had no use for more.
 *
 * It also features a function getDelta(), which will return the number of ticks
 * passed since it was called.
 *
 * Create the objects with Timer_new(), destroy them with Timer_free().
 * Start them with Timer_start(), they won't accept ticks if they hadn't been started.
 * Update them with Timer_updateDelta(), retrieve their time with Timer_getTicks().
 *
 */

#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <SDL2/SDL.h>

Uint32 getDelta();


typedef struct Timer Timer;

Timer *Timer_new();
void Timer_free(Timer *ptr);


void Timer_start(Timer *ptr);
void Timer_updateDelta(Uint32 delta, Timer *ptr);


Uint32 Timer_getTicks(Timer *ptr);

#endif // TIMER_H_INCLUDED
