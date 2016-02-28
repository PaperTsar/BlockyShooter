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

#include <stdint.h>

#ifndef DUMMY_GAMELOGIC_H
#define DUMMY_GAMELOGIC_H

/**
 * @file
 * @brief Actual game state and it's defining functions.
 * @author Bendegúz Nagy
 */

int Game_start();
void Game_func(uint32_t delta);
int Game_end();

#endif //DUMMY_GAMELOGIC_H
