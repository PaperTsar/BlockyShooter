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

#include <stdlib.h>
#include "../HEAD/timer.h"

/**
 * @brief Internal representation of time-keeping objects.
 */
struct Timer {
    Uint32 ticksRunning;
    int isStarted;
};


/**
 * @brief Used by getDelta(), holds the time the function was last called.
 */
static Uint32 lastDelta = 0;


/**
 * @brief Returns the time between now and the last time this function was called.
 * @return The time between now and the last time this function was called.
 */
Uint32 getDelta() {
    Uint32 relTime = SDL_GetTicks() - lastDelta;
    lastDelta += relTime;
    return relTime;
}

/**
 * @brief Creates a paused Timer object with zero ticks.
 * @return Returns the newly allocated Timer object.
 */
Timer *Timer_new() {
    Timer *ptr = (Timer *) malloc(sizeof(Timer));
    ptr->ticksRunning = 0;
    ptr->isStarted = 0;
    return ptr;
};

/**
 * @brief Deallocates the memory allocated by Timer_new()
 * @param ptr The Timer object to be destroyed.
 */
void Timer_free(Timer *ptr) {
    free(ptr);
}

/**
 * @brief Sets the isStarted flag of the Timer object to true.
 * @param ptr The timer object to start.
 *
 * Sets the isStarted flag of the Timer object to true. It is necessary to call this
 * for a Timer objects, otherwise it won't accept passed time updates.
 */
void Timer_start(Timer *ptr) {
    ptr->isStarted = 1;
    ptr->ticksRunning = 0;
}

/**
 * @brief Updates a Timer with a given number of ms.
 * @param delta The amount of time passed.
 * @param ptr The Timer to be updated.
 *
 * Updates a Timer with a given number of ms. The Timer has to be started for this
 * to take any effect
 */
void Timer_updateDelta(Uint32 delta, Timer *ptr) {
    if (ptr->isStarted)
        ptr->ticksRunning += delta;

}
/**
 * @brief Returns the number of ms held in the Timer object.
 * @param ptr The Timer from which the elapsed time should be extracted.
 * @return Time in ms held by this Timer object.
 */
Uint32 Timer_getTicks(Timer *ptr) {
    return ptr->ticksRunning;
}
