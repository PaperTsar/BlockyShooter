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
 * @brief Primitive physics engine.
 * @author Bendegúz Nagy
 *
 * This is a primitive physics engine implementation. I could have like used convex
 * polygons and the separating axis theorem as a basis, but I'm not smart enough for that
 * and it would be an overkill anyways.
 *
 * It has three types of objects, and all of them are AABBs.
 * Dynamic object, which can be acted upon by forces and gravity applies to them, they
 * can collide with STATIC and HYBRID objects, collision resolution applies to them exclusively.
 *
 * Hybrid objects which can collide with static and hybrid objects for the sole purpose of collision callback.
 * Gravity does not apply to them, but they can be moved by forces.
 *
 * Static objects, which do not move.
 *
 * It works by first integrating their velocity according to the forces applied to the objects
 * then integrating their position, then checking each possible combination of objects for overlap.
 * DYNAMIC vs HYBRID
 * DYNAMIC vs STATIC
 * HYBRID vs STATIC
 * HYBRID vs HYBRID
 *
 */
#ifndef DUMMY_PHYSICS_H
#define DUMMY_PHYSICS_H

#include <stdint.h>
#include "../../Utility/HEAD/vector.h"
#include "../../Utility/HEAD/bag.h"
#include "AABB.h"

/**
 * @brief Defines the type of an object, static, dynamic, hybrid.
 *
 * Defines the type of an object, static, dynamic, hybrid. Currently only these combinations are able to collide:
 *      DYNAMIC vs HYBRID
 *      DYNAMIC vs STATIC
 *      HYBRID vs STATIC
 *      HYBRID vs HYBRID
 * Collision resolution only applies to dynamic objects in the form of correcting their position for no overlap
 * with any other object. No impulse is applied. Collision callbacks apply to all of the above. Forces can only act
 * on hybrid and dynamic objects. Gravity only applies to dynamic objects.
 */
typedef enum PH_OBJ_TYPE PH_OBJ_TYPE;
/**
 * @brief Defines the context for the physics world, objects can collide with other objects from the same World.
 */
typedef struct World World;
/**
 * @brief Representation of an object in the world, only AABBs are supported currently.
 */
typedef struct Object Object;
/**
 * @brief Used in the generated PH_manifolds to identify collision types.
 *
 * Can be:
 *      DYNAMIC vs HYBRID
 *      DYNAMIC vs STATIC
 *      HYBRID vs STATIC
 *      HYBRID vs HYBRID
 */
typedef enum PH_COLL_TYPE PH_COLL_TYPE;
/**
 * @brief Holds data about a collision, used be the collision resolution function.
 */
typedef struct PH_Manifold PH_Manifold;

/**
 * @brief Objects can have collision callbacks set to them, they have to adhere to this signature.
 *
 * Object collision callback function. Return value indicates whether to collision should be allowed.
 * The second argument is the callback object, the third is the object with which it collided. The fourth argument
 * is an optional state pointer, set at callback registration.
 */
typedef int (*PH_callback)(PH_Manifold *m, Object *callObj, Object *collObj,  void *state);

/**
 * @brief Each object can have a void* userData and a userDataType bind data to objects.
 */
typedef enum UserDataType {
    /**@brief Default.*/
    NONE,
    /**@brief Destroyable piece of wall*/
    BLOCK,
    /**@brief Object representing a player*/
    PLAYER,
    /**@brief Indestructible wall blocks.*/
    WALL,
    /**@brief Attackbox spawned when a player attacks, used for collision callbacks.*/
    ATTACKBOX,
    /**@brief Bullet object spawned when the player shoots, used for collision callbacks.*/
    BULLET
} UserDataType;

/**
 * @brief Each object has one, can be set freely, type identifies the void* type.
 */
typedef struct UserData {
    UserDataType type; //helps identify the void*'s type
    void *data; //pointer to structure holding some object bound data
} UserData;

typedef struct World {
    Bag *dynObjBag; //bag for dynamic objects
    Bag *stObjBag; //bag for static objects
    Bag *hybObjBag; //bag for hybrid objects
    Vector2D gravity; //the gravity vector
    double stepTime; //the length of a single world step
    double deltaLeftover; //the remaining time which "has to be stepped yet"
} World;

typedef enum PH_OBJ_TYPE {
    STATIC = 1,
    HYBRID = 2,
    DYNAMIC = 4
} PH_OBJ_TYPE;

typedef struct Object {
    /**@brief The world this object belongs to.*/
    World *world;
    /**@brief Do not modify, index at which this object is stored in the World.*/
    int oHandle;

    PH_OBJ_TYPE type;

    Vector2D forceSum;
    Vector2D velocity;

    /**@brief Maximum velocity on the X axis.*/
    float velCapX;
    /**@brief Maximum velocity on the Y axis.*/
    float velCapY;

    float invMass;
    /**@brief The dimension of the object*/
    AABB aabb;
    /**@brief Position of the object before the last position integration.*/
    Vector2D lastPos;

    UserData userData;

    /**@brief Collision callback.*/
    PH_callback callBack;
    /**@brief Callback state passed to the callback function.*/
    void *cbState;

    /**@brief This does not belong here, used for convenient rendering.*/
    SDL_Color color;
} Object;


typedef enum PH_COLL_TYPE {
    HYBRID_HYBRID,
    STATIC_DYNAMIC,
    HYBRID_DYNAMIC,
    DYNAMIC_DYNAMIC
} PH_COLL_TYPE;


typedef struct PH_Manifold {
    Object *A;
    Object *B;
    PH_COLL_TYPE type;
    /**@brief Normal vector of the collision.*/
    Vector2D n;
    /**@brief Penetration depth.*/
    float depth;
} PH_Manifold;



World *PH_createWorld();
Object *PH_createBox(int x, int y, int width, int height, float mass, PH_OBJ_TYPE type, World *world);
void PH_setStepTime(double delta, World *world);
void PH_setGravity(float gravityX, float gravityY, World *world);
void PH_stepWorld(double delta, World *world);


void PH_impulse(Vector2D *impulse, Object *obj);
void PH_force(Vector2D *force, Object *obj);
void PH_setVelCap(float capX, float capY, Object *obj);
void PH_setPosition(Vector2D vec, Object *obj);

void PH_setUData(void *data, UserDataType type, Object *obj);

void PH_destroyObject(Object *o);
void PH_destroyWorld(World *world);


void PH_setCallback(PH_callback callBack, void *state, Object *obj);

void PH_queryPoint(Vector2D point, PH_OBJ_TYPE types, int cap, Bag *bag, World *world);



void PH_renderObjects(World *world);
void PH_setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a, Object *o);
#endif //DUMMY_PHYSICS_H
