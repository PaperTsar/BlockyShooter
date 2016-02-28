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
#include <math.h>
#include "../HEAD/AABB.h"
#include "../../Graphics/HEAD/graphics_man.h"


/**
 * @brief Checks two AABBs for overlap.
 * @param a First AABB.
 * @param b Other AABB.
 * @return 0 if they overlap, non-zero otherwise.
 */
int AABB_vs_AABB(AABB *a, AABB *b) {
    //checks if the distance on the x-axis of the two centers
    //is closer than their half-widths together
    if (fabsf(a->center.x - b->center.x) < a->hWidth + b->hWidth)
        //same but for the y axis
        if (fabsf(a->center.y - b->center.y) < a->hHeight + b->hHeight)
            return 1;

    return 0;
}

/**
 * @brief Checks if the AABB contains a given point.
 * @param a the AABB to be checked.
 * @param x the X co-ordinate of the point.
 * @param y the Y co-ordinate of the point.
 * @return 0 zero if not, non-zero otherwise
 */
int AABB_vs_Point(AABB *a, float x, float y) {
    //checks if on the x-axis, the point is in the aabb
    if (fabsf(a->center.x - x) < a->hWidth)
        //same, but for the y axis
        if ((fabsf(a->center.y - y)) < a->hHeight)
            return 1;

    return 0;
}

/**
 * @brief Convenience function for rendering an AABB with a given colour, relies on global gRenderer reference.
 * @param a the AABB to be drawn.
 * @param c the colour the AABB should be drawn with.
 */
void AABB_renderColor(AABB *a, SDL_Color c) {
    //set the color of the box by setting the draw color
    SDL_SetRenderDrawColor(gRenderer, c.r, c.g, c.b, c.a);
    //setting up a rect is required because SDL uses them to define render space
    SDL_Rect rect;
    rect.x = a->center.x - a->hWidth;
    rect.y = SCREEN_HEIGHT - a->center.y - a->hHeight;
    rect.w = a->hWidth * 2;
    rect.h = a->hHeight * 2;
    //draw the rect onto gRenderer (global variable)
    SDL_RenderFillRect(gRenderer, &rect);
}
