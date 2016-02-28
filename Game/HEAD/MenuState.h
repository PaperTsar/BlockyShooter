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

#ifndef DUMMY_MENULOGIC_H
#define DUMMY_MENULOGIC_H


#include <stdint.h>

/**
 * @file
 * @brief Main menu state and it's defining functions.
 * @author Bendegúz Nagy
 */

int Menu_start(void);
void Menu_func(uint32_t delta);
int Menu_end(void);



#endif //DUMMY_MENULOGIC_H
