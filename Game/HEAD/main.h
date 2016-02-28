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

#ifndef DUMMY_MAIN_H
#define DUMMY_MAIN_H

#include <stdint.h>

/**
 * @file
 * @brief Main holding together the whole program.
 * @author Bendegúz Nagy
 */

/** \mainpage Programozás alapjai 1 NHF - NQEHDL - Nagy Bendegúz
 *
 *  \section global Global workflow.
 *  The global state is handled by function pointers. Each state can define an init, flow and deinit
 *  function. When the program enters a state, it's init function is called, then it's flow function is called
 *  again and again. When a state is switched from, it's deinit function will be called.
 *
 *  Init and deinit function are for resource allocation and return whether it was successful, if not then the
 *  program will exit with an error. (Side note: it would be wise to define 2 more lifecycle functions, namely lazy
 *  first init and last deinit so as not to allocated resources over and over again, but i deemed it unnecessary at
 *  this scale.)
 *
 *  The program spends most of it's time in the main.c cycle which gets the passed time, updates input and timers
 *  and calls the flow function. If the exit flag is set, the program will exit (Main_setExitFlag()).
 *
 *  States can be swapped with SwapGlobalState().
 *
 *  \section init Program init.
 *  Program init is done with Main_init(), which is called once that the start of the program. Which set's up SDL,
 *  the input, timer and ttf modules and creates the function state table.
 *
 *  \section deinit Program deinit.
 *  Program deinit is don with Main_deinit(), which is called after the exit flag has been set and the program is about
 *  to exit. Calls deinit functions.
 *
 *  \section usage Brief usage.
 *   Use the arrow keys to navigate the menu, use the numpad 5,1,2,3 and arrow key to control the first player
 *   and use wasd+fcvb keys for the second player.
 *
 *  \section doc Documentation.
 *  For implementation documentation, check source code and it's comments. Each .c files has a corresponding .h file.
 *  Bigger picture is in the .h files, function documentation is in the .c files. Documentation for stuff that is defined
 *  only in the .h is in the .h file.
 */

/**
 * @brief Enumeration of possible states.
 */
typedef enum GlobalState {
    MAIN_MENU,
    LEVEL_SELECT_MENU,
    GAME,
    GLOBAL_STATE_TOTAL
} GlobalState;

/**
 * @brief Ad hoc solution for selecting a map, this shouldn't be here.
 */
extern char *currMapPath;

/**@brief State init function signature.*/
typedef int (*StateStart)(void);
/**@brief State flow function signature.*/
typedef void (*StateFunc)(uint32_t delta);
/**@brief State deinit function signature.*/
typedef int (*StateEnd)(void);


void SwapGlobalState(GlobalState stateTo);
void Main_setExitFlag();

#endif //DUMMY_MAIN_H
