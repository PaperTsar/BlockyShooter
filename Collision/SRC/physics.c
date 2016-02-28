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

#include <float.h>
#include "../HEAD/physics.h"

/**@brief No matter how much time we pass to PH_stepWorld(), it will chunk it up into this length*/
#define PH_DEF_STEPTIME (1.0/60.0)
/**@brief Prevents a sprial of death*/
#define PH_SPIRAL_OF_DEATH_CAP (0.25)



/**
 * @brief Private, integrates the position of the objects, called from PH_stepWorld(),
 */
void PH_integrate(double delta, World *world);
/**
 * @brief Private, caps the velocities, called from PH_integrate(),
 */
void PH_capVelocity(Object *obj);
/**
 * @brief Private, resets the forces to gravity or zero depending on the object type, called from PH_stepWorld(),
 */
void PH_resetForces(World *world);
/**
 * @brief Private, called from PH_stepWorld(), detects and resolves collisions.
 */
void PH_testAndResolve(World *world);
/**
 * @brief Private, used in PH_testAndResolve(), tests two objects for collision and resolves it, call callbacks.
 */
void PH_testTwoObjects(Object *A, Object *B, PH_COLL_TYPE type, PH_Manifold *m);
/**
 * @brief Private, used in PH_testTwoObjects(), tests by overlap.
 */
int PH_testOverlap(Object *A, Object *B);
/**
 * @brief Private, used in Ph_testTwoObjects(), generates collision manifold.
 */
void PH_generateManifold(Object *objA, Object *objB, PH_COLL_TYPE type, PH_Manifold *manifold);
/**
 * @brief Private, used in PH_testTwoObjects(), return whether the callbacks allow for collision, if they don't exit then they allow.
 */
int PH_testCallback(Object *A, Object *B, PH_Manifold *m);
/**
 * @brief Private, used in PH_testTwoObjects(), resolves overlap for DYNAMIC vs DYNAMIC, does nothing for anything else.
 */
void PH_resolveCollision(PH_Manifold *m);

/**
 * @brief Creates an empty world.
 */
World *PH_createWorld() {
    World *world = (World*)malloc(sizeof(World));
    //create the bag in which the object will by stored by type
    world->dynObjBag = Bag_new(&free);
    world->stObjBag = Bag_new(&free);
    world->hybObjBag = Bag_new(&free);

    //default gravity is 0
    world->gravity.x = world->gravity.y = 0;
    world->stepTime = PH_DEF_STEPTIME;
    //there is no accumulated time yet
    world->deltaLeftover = 0;
    return world;
}

/**
 * @brief Creates an objects at x,y co-ord with given heigh, width, type and mass in the given world.
 */
Object *PH_createBox(int x, int y, int width, int height, float mass, PH_OBJ_TYPE type, World *world) {
    //allocate and initilaize
    Object *box = (Object*)malloc(sizeof(Object));

    box->world = world;

    //default initialization
    box->velocity.x = box->velocity.y = 0;
    box->velCapX = FLT_MAX;
    box->velCapY = FLT_MAX;


    //setting position
    box->forceSum.x = box->forceSum.y = 0;
    box->aabb.center.y = y + height/2.0;
    box->lastPos = box->aabb.center;
    box->aabb.center.x = x + width/2.0;
    box->aabb.hWidth = width/2.0;
    box->aabb.hHeight = height/2.0;

    //empty userdata
    box->userData.type = NONE;
    box->userData.data = NULL;

    //empty callback function
    box->callBack = NULL;
    box->cbState = NULL;

    //default render colour
    box->color.r = box->color.g = box->color.b = box->color.a = 100;

    box->type = type;

    //find the correct Bag by type into which the object should be put
    switch (type) {
        //oHandle is the index at which the object is stored
        case STATIC:
            //static objects have infinity mass
            box->invMass = 0;
            box->oHandle = Bag_push(box, world->stObjBag);
            break;
        case DYNAMIC:
            box->invMass = 1.0/mass;
            box->oHandle = Bag_push(box, world->dynObjBag);
            box->forceSum = world->gravity;
            break;
        case HYBRID:
            box->invMass = 1.0/mass;
            box->oHandle = Bag_push(box, world->hybObjBag);
            break;
    }

    //return the newly allocated box
    return box;
}

/**
 * @brief Asks the world to update the objects with an amount of passed time. Collisions will be resolved and callbacks called.
 */
void PH_stepWorld(double delta, World *world) {
    //update the left over time, cap for spiral of death
    if( (world->deltaLeftover += delta) > PH_SPIRAL_OF_DEATH_CAP)
        world->deltaLeftover = PH_SPIRAL_OF_DEATH_CAP;

    //while we still time more than a stepTime chunk long to process, step the world
    while(world->deltaLeftover >= world->stepTime) {
        //integrating objects positions
        PH_integrate(world->stepTime, world);

        //resolve collisions, call callback functions
        PH_testAndResolve(world);

        //update leftover delta time, e.g. we consumed this much time
        world->deltaLeftover -= world->stepTime;
    }

    //reset forces, hybrid to zero, dynamic to gravity
    PH_resetForces(world);
}

/**
 * @brief Sets the time by which the world will be stepped.
 */
void PH_setStepTime(double delta, World *world) {
    world->stepTime = delta;
}

/**
 * @brief Sets the gravity of a world, will only have apply after the next PH_stepWorld().
 */
void PH_setGravity(float gravityX, float gravityY, World *world) {
    world->gravity.x = gravityX;
    world->gravity.y = gravityY;
}


/**
 * @brief Applies an impulse to an object.
 */
void PH_impulse(Vector2D *impulse, Object *obj) {
    obj->velocity.x += impulse->x * obj->invMass;
    obj->velocity.y += impulse->y * obj->invMass;
}

/**
 * @brief Applies a force to an object, forces are cleared after a PH_stepWorld().
 */
void PH_force(Vector2D *force, Object *obj) {
    obj->forceSum.x += force->x;
    obj->forceSum.y += force->y;
}


/**
 * @brief Deletes an object from the world, doing this during a callback will result in undefined behaviour.
 */
void PH_destroyObject(Object *o) {
    if(o == NULL)
        return;

    Bag *oBag = NULL;
    World *world = o->world;

    //select the correct bag by Object type
    switch (o->type) {
        case STATIC:
            oBag = world->stObjBag;
            break;
        case DYNAMIC:
            oBag = world->dynObjBag;
            break;
        case HYBRID:
            oBag = world->hybObjBag;
            break;
    }

    //here the handles come in handy, we can remove objects with O(1) access time
    if(oBag != NULL) {
        //remove object
        Bag_unorderedRemove(o->oHandle, oBag);
        //because we used unordered remove, we have to update the swapped object's oHandle
        //check if it wasn't the last element in the Bag
        if(o->oHandle != oBag->elemCount)
            ((Object*) oBag->vector[o->oHandle])->oHandle = o->oHandle;
        //Object is not a multi-malloc type, we can simply free it
        free(o);
    }
}

/**
 * @brief Free up all the memory the objects and the world take up.
 */
void PH_destroyWorld(World *world) {
    if(world == NULL)
        return;

    //here one means that for each object in the bag, the free function passed at Bag creation will be called
    Bag_free(world->dynObjBag, 1);
    Bag_free(world->hybObjBag, 1);
    Bag_free(world->stObjBag, 1);
    free(world);
}


/**
 * @brief Sets the velocity cap, calling this during a callback is undefined.
 */
void PH_setVelCap(float capX, float capY, Object *obj) {
    obj->velCapX = capX;
    obj->velCapY = capY;
}

/**
 * @brief Sets a the position of an object, calling this during a callback is undefined.
 */
void PH_setPosition(Vector2D vec, Object *obj) {
    obj->lastPos = obj->aabb.center;

    obj->aabb.center.x = vec.x + obj->aabb.hWidth;
    obj->aabb.center.y = vec.y + obj->aabb.hHeight;
}

/**
 * @brief Sets the userdata and it's type for an object.
 */
void PH_setUData(void *data, UserDataType type, Object *obj) {
    obj->userData.data = data;
    obj->userData.type = type;
}

/**
 * @brief Set the callback function for an object and it's related state pointer.
 */
void PH_setCallback(PH_callback callBack, void *state, Object *obj) {
    obj->cbState = state;
    obj->callBack = callBack;
}

/**
 * @brief Set the color of the object, can be used for convenient rendering.
 */
void PH_setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a, Object *o) {
    o->color.r = r;
    o->color.g = g;
    o->color.b = b;
    o->color.a = a;
}

/**
 * @brief Render the objects by their colours.
 */
void PH_renderObjects(World *world) {
    //these are for iterating over elements
    int i; //array index iterator
    int elemCount = 0; //will store the number of elements in a Bag
    Object **objVector = NULL; //will store a type cast array


    objVector = (Object**)world->stObjBag->vector; //cache the Bag's backing array
    elemCount = world->stObjBag->elemCount; //cache the Bag's element count
    for(i=0;i<elemCount;i++)
        AABB_renderColor(&(objVector[i]->aabb), objVector[i]->color);


    objVector = (Object**)world->dynObjBag->vector; //cache the Bag's backing array
    elemCount = world->dynObjBag->elemCount; //cache the Bag's element count
    for(i=0;i<elemCount;i++)
        AABB_renderColor(&(objVector[i]->aabb), objVector[i]->color);


    objVector = (Object**)world->hybObjBag->vector; //cache the Bag's backing array
    elemCount = world->hybObjBag->elemCount; //cache the Bag's element count
    for(i=0;i<elemCount;i++)
        AABB_renderColor(&(objVector[i]->aabb), objVector[i]->color);

}


/**
 * @brief Clear and fill the passed Bag* with the Objects that contain the point.
 * @param PH_OBJ_TYPE can be used for type specification, OR'ing together types
 * @param cap can be used for specifing max find count, negative if no cap
 */
void PH_queryPoint(Vector2D point, PH_OBJ_TYPE types, int cap, Bag *bag, World *world) {
    //i is the array index iterator, found holds the number of elements found
    int i, found = 0;
    int elemCount = 0; //Bag elemcount
    Object **vector = NULL; //Bag's backing array

    Bag_fastClear(bag);


    if(types & DYNAMIC) {
        elemCount = world->dynObjBag->elemCount; //cache Bag's element count
        vector = (Object**)world->dynObjBag->vector; //cache Bag's backing array
        //while the array lasts and if a cap has been specified
        for(i = 0; i < elemCount && (found < cap || cap <= 0); i++) {
            if(AABB_vs_Point(&(vector[i]->aabb), point.x, point.y)) {
                Bag_push(vector[i], bag);
            }
        }
    }

    if(types & HYBRID) {
        elemCount = world->hybObjBag->elemCount;
        vector = (Object**)world->hybObjBag->vector;
        for(i = 0; i < elemCount && (found < cap || cap <=0); i++) {
            if(AABB_vs_Point(&(vector[i]->aabb), point.x, point.y)) {
                Bag_push(vector[i], bag);
            }
        }
    }

    if(types & STATIC) {
        elemCount = world->stObjBag->elemCount;
        vector = (Object**)world->stObjBag->vector;
        for(i = 0; i < elemCount && (found < cap || cap<=0); i++) {
            if(AABB_vs_Point(&(vector[i]->aabb), point.x, point.y)) {
                Bag_push(vector[i], bag);
            }
        }
    }
}





//private methods

void PH_integrate(double delta, World *world) {
    //helper local iterators
    int i;
    int elemCount = 0;
    Object **objs = NULL;

    //add force to velocity, dynamic objects
    elemCount = world->dynObjBag->elemCount;
    objs = (Object**)world->dynObjBag->vector;
    for(i = 0; i<elemCount; i++) {
        //update velocity
        objs[i]->velocity.x += objs[i]->forceSum.x * objs[i]->invMass * delta;
        objs[i]->velocity.y += objs[i]->forceSum.y * objs[i]->invMass * delta;

        //cap velocity
        PH_capVelocity(objs[i]);
    }

    //integrate dynamic object positions
    for(i=0;i<elemCount;i++){
        //save last pos
       objs[i]->lastPos = objs[i]->aabb.center;

        //update poisition
       objs[i]->aabb.center.x += objs[i]->velocity.x * delta;
       objs[i]->aabb.center.y += objs[i]->velocity.y * delta;
    }


    //add force to velocity, hybrid
    elemCount = world->hybObjBag->elemCount;
    objs = (Object**)world->hybObjBag->vector;
    for(i = 0; i<elemCount; i++) {
        //update velocity
        objs[i]->velocity.x += objs[i]->forceSum.x * objs[i]->invMass * delta;
        objs[i]->velocity.y += objs[i]->forceSum.y * objs[i]->invMass * delta;

        //cap velocity
        PH_capVelocity(objs[i]);
    }


    //integrate hybrid object positions
    for(i=0;i<elemCount;i++){
        //save last pos
        objs[i]->lastPos = objs[i]->aabb.center;
        //update position
        objs[i]->aabb.center.x += objs[i]->velocity.x * delta;
        objs[i]->aabb.center.y += objs[i]->velocity.y * delta;
    }
}

void PH_capVelocity(Object *obj) {
    //cache the object's velocity
    Vector2D *vel = &(obj->velocity);
    //cache the caps
    float capX = obj->velCapX;
    float capY = obj->velCapY;

    //check if bigger than the cap, if yes cap it with regards to negative numbers
    if(fabsf(vel->x) > capX)
        vel->x = vel->x > 0 ? capX : -capX;
    if(fabsf(vel->y) > capY)
        vel->y = vel->y > 0 ? capY : -capY;
}


void PH_testAndResolve(World *world) {
    //iterators
    int i, j;
    int elemCountOut = 0, elemCountIn = 0;
    Object **vectorOut = NULL, **vectorIn = NULL;

    //used in the inner loop
    PH_Manifold m;
    Object *A = NULL, *B = NULL;


    //the inner data loop is always dynamic objects
    elemCountIn = world->dynObjBag->elemCount;
    vectorIn = (Object**)world->dynObjBag->vector;

    //dynamic vs dynamic
    elemCountOut = world->dynObjBag->elemCount;
    vectorOut = (Object**)world->dynObjBag->vector;
    for(i = 0; i<elemCountOut - 1; i++) {
        for(j = i+1; j<elemCountIn; j++) {
            PH_testTwoObjects(vectorOut[i], vectorIn[j], DYNAMIC_DYNAMIC, &m);
        }
    }

    //hybrid vs dynamic
    elemCountOut = world->hybObjBag->elemCount;
    vectorOut = (Object**)world->hybObjBag->vector;
    for(i = 0; i<elemCountOut; i++) {
        for(j = 0; j<elemCountIn; j++) {
            PH_testTwoObjects(vectorOut[i], vectorIn[j], HYBRID_DYNAMIC, &m);
        }
    }

    //static vs dynamic
    elemCountOut = world->stObjBag->elemCount;
    vectorOut = (Object**)world->stObjBag->vector;
    for(i = 0; i<elemCountOut; i++) {
        for(j = 0; j<elemCountIn; j++) {
            PH_testTwoObjects(vectorOut[i], vectorIn[j], STATIC_DYNAMIC, &m);
        }
    }

    //except in this case, inner is not dynamic
    //hybrid vs hybrid
    elemCountIn = elemCountOut = world->hybObjBag->elemCount;
    vectorIn = vectorOut = (Object**)world->hybObjBag->vector;
    for(i = 0; i<elemCountOut - 1; i++) {
        for(j = i + 1; j<elemCountIn; j++) {
            PH_testTwoObjects(vectorOut[i], vectorIn[j], HYBRID_HYBRID, &m);
        }
    }
}

void PH_testTwoObjects(Object *A, Object *B, PH_COLL_TYPE type, PH_Manifold *m) {
    //test if the two object are overlapping
    if (PH_testOverlap(A, B)) {
        //generate manifold first, because the callback functions might need it
        PH_generateManifold(A, B, type, m);
        //after generating manifold, we ask the callback functions (if thy exits)
        //do their whatever and have them return if the two object should collide
        if (PH_testCallback(A, B, m))
            PH_resolveCollision(m);

    }
}

int PH_testOverlap(Object *A, Object *B) {
    if (AABB_vs_AABB(&(A->aabb), &(B->aabb)) != 0)
        return 1;

    return 0;
}




void PH_generateManifold(Object *objA, Object *objB, PH_COLL_TYPE type, PH_Manifold *dest) {
    //set the manifolds collided objects pointers
    dest->A = objA;
    dest->B = objB;
    dest->type = type;

    //chache the objects' defining AABBs
    AABB *A = &(objA->aabb);
    AABB *B = &(objB->aabb);

    //get the normal vector
    dest->n = VEC2D_sub(&(B->center), &(A->center));

    //calculate overlap on x and y axis
    float dx = A->hWidth + B->hWidth - fabsf(dest->n.x);
    float dy = A->hHeight + B->hHeight - fabsf(dest->n.y);

    //set the manifold values according to whether the overlap is "shorter"
    //on the x or y axis
    if(dx < dy) {
        dest->depth = dx;
        dest->n.y = 0;
        dest->n.x = dest->n.x < 0 ? -1 : 1;
    } else {
        dest->depth = dy;
        dest->n.x = 0;
        dest->n.y = dest->n.y < 0 ? -1 : 1;
    }

}

//call the callback functions and return wheter they disallow the collision
int PH_testCallback(Object *A, Object *B, PH_Manifold *m) {
    int en = 1;

    //check if the callback function exists
    if(A->callBack != NULL)
        //if it returns 0, set enable to zero
        if(!(A->callBack(m, A, B, A->cbState)))
            en = 0;

    //same as above
    if(B->callBack != NULL)
        if(!(B->callBack(m, B, A, B->cbState)))
            en = 0;

    return en;
}


void PH_resolveCollision(PH_Manifold *m) {
    Object *A = m->A;
    //B is always a dynamic object, except when it's hybrid in hybrid vs hybrid
    Object *B = m->B;

    switch (m->type) {
        case STATIC_DYNAMIC:
        case HYBRID_DYNAMIC:
            if(m->n.x != 0){
                B->aabb.center.x += m->n.x * m->depth;
                B->velocity.x = 0;
            } else {
                B->aabb.center.y += m->n.y * m->depth;
                B->velocity.y = 0;
            }
            break;
    }
}



/*
 * Clear forces for hybrids and dynamic, set dynamic forceSum to gravity.
 */

void PH_resetForces(World *world) {
    //helper local variables
    int i;
    int elemCount = world->dynObjBag->elemCount;
    Object **objvector = (Object**)(world->dynObjBag->vector);

    //reset dynamic objects' forces to gravity
    for(i=0;i<elemCount;i++){
        objvector[i]->forceSum = VEC2D_scale(&(world->gravity), 1.0/objvector[i]->invMass);
    }

    //reset hybrid objects' forces to zero
    elemCount = world->hybObjBag->elemCount;
    objvector = (Object**)(world->hybObjBag->vector);
    for(i=0; i<elemCount; i++) {
        objvector[i]->forceSum.x = 0;
        objvector[i]->forceSum.y = 0;
    }
}
