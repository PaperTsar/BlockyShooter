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
 * @brief Dynamically growing array implementation.
 * @author Bendegúz Nagy
 *
 * Create a new dynamic array with Bag_new(), add element with Bag_push(), remove with
 * Bag_unordered() remove. Iterate by accessing the implementation. Search by pointer equality
 * with Bag_search()
 */

#ifndef DUMMY_BAG_H
#define DUMMY_BAG_H

/**
 * @brief Bags can have a data freeing function which have to adhere to this signature.
 */
typedef void (*freeData)(void *ptr);

/**
 * @brief Holds a dynamically growing array.
 */
typedef struct Bag {
    void **vector;
    int elemCount;
    int maxSize;
    freeData freeDataPtr;
} Bag;

Bag *Bag_new(freeData freeDatPtr);
void Bag_free(Bag *bag, int freeData);

int Bag_push(void *data, Bag *bag);
void *Bag_unorderedRemove(int index, Bag *bag);
int Bag_search(void *data, Bag *bag);
void Bag_slowClear(Bag *bag, int free);
void Bag_fastClear(Bag *bag);

#endif //DUMMY_BAG_H
