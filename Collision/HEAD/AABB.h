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
 * @brief Module for representing rectangles by means of half widths and heights and the center co-ordinates.
 * @author Bendegúz Nagy
 *
 * Create new AABBs on the stack or by means of dynamic memory allocation.
 */

#ifndef DUMMY_AABB_H
#define DUMMY_AABB_H

#include <SDL2/SDL.h>
#include "../../Utility/HEAD/vector.h"

/**
 * @brief Represents a rectangle by half width, height and the center co-ordinates.
 */
typedef struct AABB {
    Vector2D center;
    float hWidth;
    float hHeight;
} AABB;

int AABB_vs_AABB(AABB *a, AABB *b);

int AABB_vs_Point(AABB *a, float x, float y);

void AABB_renderColor(AABB *a, SDL_Color c);

#endif //DUMMY_AABB_H
