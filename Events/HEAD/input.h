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
 * @brief Module for handling SDL input with push-down implementation.
 * @author Bendegúz Nagy
 *
 * This module maintains an ordered list of inputConsumer functions. When Input_process() is called,
 * the module calls SDL_PollEvent() until there are no events left and feeds the SDL_Events to the
 * registered inputConsumers. If one of those functions returns non-zero, then the SDL_Event is considered
 * consumed and will not be passed to successive functions in the list.
 *
 * Initialize the module with Input_init(), deinitialize with Input_deinit().
 * When you wish to process the events accumulated in SDL, call Input_process().
 * Register inputConsumers with Input_subscribe().
 * To empty the maintained list, call Input_clear().
 */

#ifndef DUMMY_INPUT_H
#define DUMMY_INPUT_H


/**
 * @brief Registered functions have to adhere to this signature.
 * @param e Pointer to the SDL_Event to be processed.
 * @param state Optional state pointer set at function registration to be passed to the function with each call
 * @return 0 if the event should be considered consumed, non-zero otherwise
 */
typedef int (*inputConsumer)(SDL_Event *e, void *state);



void Input_init();
void Input_deinit();


void Input_subscribe(inputConsumer cons, void *state);
void Input_process();


void Input_clear();



#endif //DUMMY_INPUT_H
