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

/**
 * @file
 * @brief Handles the initialization of SDL, creation of the window and loading of png files.
 * @author Bendegúz Nagy
 *
 * Initialize SDL and create window with GM_init(), deinitialize with GM_deinit(). Load png files into
 * SDL_Textures with GM_loadPngFromFile().
 */

#ifndef GRAPHICS_MAN_H_INCLUDED
#define GRAPHICS_MAN_H_INCLUDED

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;


int GM_init(int SDL_FlAGS, int IMG_FLAGS);
void GM_deinit();


SDL_Texture *GM_loadPngFromFile(char *path, int r, int g, int b);

#endif // GRAPHICS_MAN_H_INCLUDED
