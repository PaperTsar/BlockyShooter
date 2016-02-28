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

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"


/**
 * @file
 * @brief Module for rendering text.
 * @author Bendegúz Nagy
 *
 * This module uses a single global font type and lazy initialization for requested text sizes. Initialize the module with
 * TS_init(), deinit with TS_deinit(). Create new empty text objects with TS_new(), free them with TS_free(). Set their
 * text any number of times with TS_setText(). Set their position with TS_setPos(), render them with TS_render().
 */

#ifndef TEXTSPRITE_H_INCLUDED
#define TEXTSPRITE_H_INCLUDED


typedef struct TextSprite TextSprite;


void TS_init(char *fontPath);
void TS_deinit();

TextSprite *TS_new();
void TS_free(TextSprite *ptr);

int TS_setText(char *text, SDL_Color *color, int size, TextSprite *ptr);
void TS_setPos(int x, int y, TextSprite *ptr);

void TS_render(TextSprite *ptr);


int TS_getWidth(TextSprite *ptr);
int TS_getHeight(TextSprite *ptr);
#endif // TEXTSPRITE_H_INCLUDED
