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
 * @brief Basic library for handling 2D vectors represented by float co-ordinates.
 * @author Bendegúz Nagy
 */


#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

/**
 * @brief Represents two vectors in floats, for performance reasons.
 */
typedef struct {
    float x;
    float y;
} Vector2D;

/**
 * @brief Allocates a new Vector.
 * @param x initial x coordinate.
 * @param y initial y coordinate.
 * @return pointer to the allocated vector.
 */
Vector2D *VEC2D_new(float x, float y);

/**
 * @brief Allocates a new vector.
 * @param angle the initial angle.
 * @param length the initial length.
 * @return pointer to the allocated vector.
 */
Vector2D *VEC2D_Pnew(float angle, float length);

/**
 * @brief Deallocates a vector allocated by either VEC2D_Pnew() or VEC2D_new().
 * @param ptr the vector to be freed.
 */
void VEC2D_free(Vector2D *ptr);

/**
 * @brief Adds two vectors together.
 * @return the resulting vector.
 */
Vector2D VEC2D_add(const Vector2D *srcA, const Vector2D *srcB);

/**
 * @brief Subtract the second vector from the first vector.
 * @param srcA the second to be subtracted from.
 * @return the resulting vector.
 */
Vector2D VEC2D_sub(const Vector2D *srcA, const Vector2D *srcB);

/**
 * @brief Scale a vector with a given scalar.
 * @return the resulting vector.
 */
Vector2D VEC2D_scale(Vector2D *srcA, float scale);

/**
 * @brief Rotate a vector by a given angle.
 * @return the resulting vector.
 */
Vector2D VEC2D_rotate(Vector2D *srcA, float angle);

/**
 * @brief Normalize a vector.
 * @return the resulting vector.
 */
Vector2D VEC2D_normalize(Vector2D *srcA);

/**
 * @brief Calculate the distance between two points defined by two vectors.
 * @return the distance.
 */
float VEC2D_distance(Vector2D *a, Vector2D *b);

/**
 * @brief Calculate the dot product of from two vectors.
 * @return the calculated dot product.
 */
float VEC2D_scalar(Vector2D *a, Vector2D *b);

/**
 * @brief Get the angle between a vector and the X axis.
 * @return the angle of a vector.
 */
float VEC2D_angle(Vector2D *src);

/**
 * @brief Get the length of a vecotor.
 * @return the length of a vector.
 */
float VEC2D_length(Vector2D *src);

#endif // VECTOR_H_INCLUDED

