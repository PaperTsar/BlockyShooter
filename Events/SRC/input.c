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

#include <SDL_events.h>
#include "../HEAD/input.h"
#include "../../Utility/HEAD/bag.h"


/**
 * @brief Internal representation of subscribed functions.
 */
typedef struct Subscriber {
    inputConsumer consFuc;
    void *state;
} Subscriber;

/**
 * @brief Internal list holding the subscribed functions.
 */
static Bag *subscribers = NULL;

/**
 * @brief Initializes the module.
 *
 * Initializes the module. It is mandatory to call this before the module is put to use in any way.
 * Calling this more than once without calling Input_deinit() in between will cause a memory leak.
 */
void Input_init()
{
    subscribers = Bag_new(&free);
}

/**
 * @brief Deinitializes the module.
 */
void Input_deinit()
{
    Bag_free(subscribers, 1);
}

/**
 * @brief Resets the list of subscribed functions.
 */
void Input_clear()
{
    Bag_slowClear(subscribers, 1);
}

/**
 * @brief Subscribe functions and their state pointers with this.
 * @param cons Function pointer to the inputConsumer function.
 * @param state Optional state pointer to be passed to the function with each call.
 *
 * Subscribe functions and their state pointers with this. The processing will be done
 * in the order the functions are registered.
 */
void Input_subscribe(inputConsumer cons, void *state)
{
    Subscriber *sub = (Subscriber *) malloc(sizeof(Subscriber));
    sub->consFuc = cons;
    sub->state = state;
    Bag_push(sub, subscribers);
}


/**
 * @brief Process accumulated SDL_Events by pushing them down the inputConsumer queue.
 */
void Input_process()
{
    //Cache some values locally for iteration.
    int elemCount = subscribers->elemCount;
    Subscriber **subs = (Subscriber **) subscribers->vector;
    int i;
    SDL_Event e;

    //If an inputConsumer returns non-zero, the SDL_Event will be considered consumed and will not
    //be passed to successive inputConsumers in the list.
    while (SDL_PollEvent(&e))
        for (i = 0; i < elemCount; i++)
            if ((subs[i]->consFuc(&e, subs[i]->state)) == 1)
                break;

}
