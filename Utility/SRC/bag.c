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
#include <string.h>
#include "../HEAD/bag.h"

/**
 * @brief The initial size of a bag.
 */
#define BAG_INIT_SIZE (16)
/**
 * @brief Scale at which the bag will grow if required.
 */
#define BAG_GROW_RATE (7.0/4.0)

/**
 * @brief Internal function to increase the size of a Bag.
 */
void AS_grow(Bag *stack);

/**
 * @brief Allocates a new Bag.
 * @param freeDataPtr a function that will be used to free held data when deleting elements.
 * @return the newly allocated Bag.
 */
Bag *Bag_new(freeData freeDataPtr) {
    Bag *stack = (Bag*)malloc(sizeof(Bag));
    stack->vector = (void**)calloc(sizeof(void*), BAG_INIT_SIZE);
    stack->maxSize = BAG_INIT_SIZE;
    stack->elemCount = 0;
    stack->freeDataPtr = freeDataPtr;
    return stack;
}

/**
 * @brief Deallocate a bag allocated by Bag_new().
 * @param bag the bag to be destroyed.
 * @param freeDataPtr non-zero if the bag should call the registered free function for each contained data pointer.
 */
void Bag_free(struct Bag *bag, int freeDataPtr) {
    if(bag == NULL)
        return;

    //free with the freeFunction
    int i;
    if(freeDataPtr && bag->freeDataPtr)
        for(i = 0; i < bag->elemCount; i++)
            bag->freeDataPtr(bag->vector[i]);

    //free the bag
    free(bag->vector);
    free(bag);
}

/**
 * @brief Push a new element to the end of the Bag.
 * @return the index at which the new data will be stored, can change when elements are deleted.
 */
int Bag_push(void *data, struct Bag *bag) {
    //grow the bag if it has reached it's maximum capacity
    if(bag->elemCount == bag->maxSize)
        AS_grow(bag);

    bag->vector[(bag->elemCount)++] = data;

    //return the index of the new element
    return bag->elemCount - 1;
}

/**
 * @brief Remove an element at an index, place the last element in it's place.
 * @return the removed data pointer.
 */
void *Bag_unorderedRemove(int index, Bag *bag) {
    //cache the element to be removed, to be later returned
    void *element = bag->vector[index];
    //replace at index with the last element, decrease element count
    bag->vector[index] = bag->vector[--(bag->elemCount)];
    //overwrite with NULL the last element, for error detection
    bag->vector[bag->elemCount] = NULL;

    return element;
}

/**
 * @brief Linear search by pointer equality.
 * @return the index at which the data is stored, -1 if it could not be found.
 */
int Bag_search(void *data, Bag *bag) {
    int i;
    for(i=0; i<bag->elemCount; i++)
        if(data == bag->vector[i])
            return i;

    return -1;
}
/**
 * @brief Clears the bag and overwrites the backing array with zeros.
 * @param free if the registered free function should be called for each contained data pointer.
 */
void Bag_slowClear(Bag *bag, int free) {
    //call free function for each element
    int i;
    if(free && bag->freeDataPtr)
        for(i=0; i<bag->elemCount; i++)
            bag->freeDataPtr(bag->vector[i]);

    //overwrite array with zeroes
    memset(bag->vector, 0, bag->maxSize);
    //set elementcount to zero
    bag->elemCount = 0;
}

/**
 * @brief Sets the element count to zero.
 */
void Bag_fastClear(Bag *bag) {
    bag->elemCount = 0;
}


// private methods


void AS_grow(Bag *stack) {
    void **newVector = (void**)malloc(sizeof(void*) * stack->maxSize * BAG_GROW_RATE);
    memcpy(newVector, stack->vector, sizeof(void*) * stack->elemCount);
    free(stack->vector);

    stack->vector = newVector;
    stack->maxSize *= BAG_GROW_RATE;
}
