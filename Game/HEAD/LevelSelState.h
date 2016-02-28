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

#ifndef DUMMY_LEVELSELSTATE_H
#define DUMMY_LEVELSELSTATE_H

/**
 * @file
 * @brief Level select menu state and it's defining functions.
 * @author Bendegúz Nagy
 *
 * This is almost exact copy of the MenuState state. For information on how this works, check that out.
 */


#include <stdint.h>

int LevelSel_start();
void LevelSel_func(uint32_t delta);
int LevelSel_end();


#endif //DUMMY_LEVELSELSTATE_H
