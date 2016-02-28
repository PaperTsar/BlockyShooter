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

#include "../HEAD/vector.h"
#include <stdlib.h>
#include <math.h>

Vector2D *VEC2D_new(float x, float y) {
    Vector2D *ptr = (Vector2D *) malloc(sizeof(Vector2D));
    ptr->x = x;
    ptr->y = y;
    return ptr;
}

Vector2D *VEC2D_Pnew(float angle, float length) {
    Vector2D *ptr = (Vector2D *) malloc(sizeof(Vector2D));
    ptr->x = cosf(angle) * length;
    ptr->y = sinf(angle) * length;
    return ptr;
}


void VEC2D_free(Vector2D *ptr) {
    free(ptr);
}


Vector2D VEC2D_add(const Vector2D *srcA, const Vector2D *srcB) {
    Vector2D vec = {
            srcA->x + srcB->x,
            srcA->y + srcB->y
    };
    return vec;
}

Vector2D VEC2D_sub(const Vector2D *srcA, const Vector2D *srcB) {
    Vector2D vec = {
            srcA->x - srcB->x,
            srcA->y - srcB->y
    };
    return vec;
}

Vector2D VEC2D_scale(Vector2D *srcA, float scale) {
    Vector2D vec = {
            srcA->x * scale,
            srcA->y * scale
    };
    return vec;
}

Vector2D VEC2D_rotate(Vector2D *srcA, float angle) {
    float x = srcA->x, y = srcA->y;
    Vector2D vec = {
            x * cosf(angle) - y * sinf(angle),
            x * sinf(angle) + y * cosf(angle)
    };
    return vec;
}

Vector2D VEC2D_normalize(Vector2D *srcA) {
    float length = VEC2D_length(srcA);
    return VEC2D_scale(srcA, 1.0 / length);
}


Vector2D VEC2d_integrate(Vector2D *srcA, Vector2D *srcInteg, float scale) {
    Vector2D vec = {
            srcA->x + srcInteg->x * scale,
            srcA->y + srcInteg->y * scale
    };
    return vec;
}

float VEC2D_distance(Vector2D *a, Vector2D *b) {
    Vector2D tmp = VEC2D_sub(a, b);
    return VEC2D_length(&tmp);
}

float VEC2D_scalar(Vector2D *a, Vector2D *b) {
    return a->x * b->x + a->y * b->y;
}

float VEC2D_angle(Vector2D *src) {
    return atan(src->y / src->x);
}

float VEC2D_length(Vector2D *src) {
    return sqrt(src->x * src->x + src->y * src->y);
}

float VEC2d_lSquared(Vector2D *src) {
    return src->x * src->x + src->y * src->y;
}
