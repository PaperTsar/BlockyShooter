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

#include "../HEAD/player.h"
#include "../../Events/HEAD/Timer_man.h"

#define XCAP 350
#define YCAP 1000

#define WALK_FORCE 2100
#define JUMP_SPEED 700
#define FLY_FORCE 1600
#define SLIDE_MAX 85


#define ATTACK_DURR 150
#define ATTACK_CD 500
#define CLING_IMPULSE 700

#define DASH_CD 1000
#define DASH_SPEED 1500
#define DASH_DURR  50

#define SHOOT_CD 300
#define BULLET_SPEED 1000
#define SHOOT_COUNT 3000



/**
 * @brief Helper bag fro querying stuff, do not assume it's contents will remain the same in between function calls.
 */
Bag *queryBag = NULL;
/**
 * @brief Only push and only objects onto this. Holds PH_Objects which could not be deleted during a callback.
 */
Bag *destroyBag = NULL;

/**
 * @brief This has to be called before the player module is put to use. Calling this multiple times without calling
 * Player_deinitModule() in between will cause a memory leak.
 */
void Player_initModule() {
    queryBag = Bag_new(NULL);
    destroyBag = Bag_new(NULL);
}
/**
 * @brief Deinitializes the player module.
 */
void Player_deinitModule() {
    Bag_free(queryBag, 0);
    Bag_free(destroyBag, 0);
    queryBag = destroyBag = NULL;
}

/**
 * @brief Defines a timer callback function for pulling the player out of the attacking state and destroying it's attackbox.
 */
int Player_attackRemoveTimer(Uint32 delta, Timer *timer, Player *p);
/**
 * @brief PH_callback for the attackbox collisions.
 */
int Player_attackBoxColl(PH_Manifold *m, Object *callObj, Object *collObj, Player *p);
/**
 * @brief PH_callback for player collisions.
 */
int Player_collCallBack(PH_Manifold *m, Object *A, Object *B, Player *player);
/**
 * @brief Defines a timer callback function for pulling the player out of the dashing state.
 */
int Player_dashTimer(Uint32 delta, Timer *timer, Player *p);
/**
 * @brief PH_callback for bullets.
 */
int Player_bulletCB(PH_Manifold *m, Object *A, Object *B, Player *p);

//logic state functions
void Player_still(Player *p);
void Player_walking(Player *p);
void Player_goingUp(Player *p);
void Player_goingDown(Player *p);
void Player_attack(Player *p);
void Player_dead(Player *p);
void Player_dash(Player *p);
void Player_shoot(Player *p);

//movement state machine functions, NULL means no mevement
void Player_groundMov(Player *p);
void Player_flyMov(Player *p);

//note, the freakin' order counts
//if you switch it up, don't come crying to yourself
/**
 * @brief Array holding the state functions.
 */
const stateFunc states[8] = {
    &Player_still,
    &Player_walking,
    &Player_goingUp,
    &Player_goingDown,
    &Player_attack,
    &Player_dead,
    &Player_dash,
    &Player_shoot
};

//same as above, switch it and you'll be in trouble, big time, srsly tho
/**
 * @brief Array for holding movement state functions.
 */
const stateFunc movStates[3] = {
        NULL,
        &Player_flyMov,
        &Player_groundMov
};

/**
 * @brief Create a new player with no color or controlling keys.
 */
Player *Player_new(int x, int y, World *world) {
    Player *player = (Player*)malloc(sizeof(Player));

    player->world = world;
    player->phObj = PH_createBox(x, y, 32, 32, 1, DYNAMIC, world);
    PH_setUData(player, PLAYER, player->phObj);
    PH_setCallback((PH_callback)&Player_collCallBack, player, player->phObj);

    PH_setVelCap(XCAP, YCAP, player->phObj);
    player->shData.bag = Bag_new(NULL);
    Player_reset(player);
    player->score = 0;


    return player;
}

/**
 * @brief Resets a player, can be used like after a respawn.
 */
void Player_reset(Player *p) {
    Player_setState(GOING_DOWN, p);
    Player_setMovState(FLY, p);
    p->keyDown = p->contKeyDown =
    p->flags = p->attData.attCD =
    p->dashData.dashCD = p->shData.shootCD =
    p->dashData.isLive = 0;
    p->shData.shootCount = SHOOT_COUNT;
    p->attData.box = NULL;
    p->attData.isLive = 0;
    p->attData.usedUp = 0;

    int i;
    for(i=0; i<p->shData.bag->elemCount; i++)
        PH_destroyObject(p->shData.bag->vector[i]);
    Bag_fastClear(p->shData.bag);
}

/**
 * @brief Use this for swapping a player's state.
 */
void Player_setState(PLAYER_STATE state, Player *p) {
    p->state = states[state];
    p->flags |= STATE_INIT;
}

/**
 * @brief Use this to compare the player's state to a state enum.
 */
int Player_compState(PLAYER_STATE state, Player *p) {
    if(p->state == states[state])
        return 1;

    return 0;
}

/**
 * @brief Use this for setting the movement state.
 */
void Player_setMovState(PLAYER_MOV_STATE state, Player *p) {
    p->movState = movStates[state];
    p->flags |= MOV_STATE_INIT;
}

/**
 * @brief Fill's the player's arrays with the SDL_Keycodes.
 */
void Player_setControl(SDL_Keycode up, SDL_Keycode down, SDL_Keycode left, SDL_Keycode right,
                       SDL_Keycode attack, SDL_Keycode jump, SDL_Keycode dash, SDL_Keycode shoot, Player *player) {
    player->keys[0] = up;
    player->keys[1] = down;
    player->keys[2] = left;
    player->keys[3] = right;
    player->keys[4] = attack;
    player->keys[5] = jump;
    player->keys[6] = dash;
    player->keys[7] = shoot;
}

/**
 * @brief Deallocates a player.
 */
void Player_free(Player *player) {
    PH_destroyObject(player->phObj);
    PH_destroyObject(player->attData.box);
    Bag_free(player->shData.bag, 0);
    free(player);
}

/**
 * @brief Input consumer for a player.
 */
int Player_feedInput(SDL_Event *e, Player *p) {
    //if the event is not of type 'key', this will contain garbage
    SDL_Keycode k = e->key.keysym.sym;
    int repeat = e->key.repeat;
    //this should only be used for checking if change has occurred
    uint32_t prevContKeyDown = p->contKeyDown;
    //this should only be used for checking if change has occurred
    uint32_t kD = p->keyDown;

    switch (e->type) {
        case SDL_KEYDOWN: {
            if (k == p->keys[0]) {
                p->contKeyDown |= MOV_UP;
            } else if (k == p->keys[1]) {
                p->contKeyDown |= MOV_DOWN;
            } else if (k == p->keys[2]) {
                p->contKeyDown |= MOV_LEFT;
            } else if (k == p->keys[3]) {
                p->contKeyDown |= MOV_RIGHT;
            } else if (k == p->keys[4] && !repeat) {
                p->keyDown |= ATT_KEY;
            } else if (k == p->keys[5] && !repeat) {
                p->keyDown |= JUMP_KEY;
            } else if (k == p->keys[6] && !repeat) {
                p->keyDown |= DASH_KEY;
            } else if (k == p->keys[7] && !repeat) {
                p->keyDown |= SHOOT_KEY;
            }
        }
            break;
        case SDL_KEYUP: {

            if (k == p->keys[0]) {
                p->contKeyDown &= ~MOV_UP;
            } else if (k == p->keys[1]) {
                p->contKeyDown &= ~MOV_DOWN;
            } else if (k == p->keys[2]) {
                p->contKeyDown &= ~MOV_LEFT;
            } else if (k == p->keys[3]) {
                p->contKeyDown &= ~MOV_RIGHT;
            }
        }
    }

    //returns if there has been a change, eg. the event had been consumed
    return kD != p->keyDown || p->contKeyDown != prevContKeyDown;
}

int Player_collCallBack(PH_Manifold *m, Object *A, Object *B, Player *player) {
    //if the player collided with a 'solid' object and was moving downward, then he is on the ground
    if(B->type == STATIC || B->type == HYBRID) {
        Vector2D vec = VEC2D_sub(&A->aabb.center, &A->lastPos);
        if(vec.y < 0)
            player->flags |= ON_THE_GROUND;

    }

    //player collides with everything
    return 1;
}

/**
 * @brief Call this function after rendering has been compelted.
 */
void Player_postRender(Uint32 delta) {
    //destroy the objects which should have been destroyed in a
    //PH_callback, but could not have been because ... it was in a PH_callback
    int i;
    for (i = 0; i<destroyBag->elemCount; i++)
        PH_destroyObject(destroyBag->vector[i]);
    Bag_fastClear(destroyBag);
}
/**
 * @brief Updates a player, does like calling the state and movement function.
 */
void Player_update(Player *p, Uint32 delta) {
    //save previous states to later check if there has been a change
    stateFunc prevState = p->state;
    stateFunc prevMovState = p->movState;

    //update player cooldowns
    if((p->attData.attCD -= delta) < 0)
        p->attData.attCD = 0;
    if((p->dashData.dashCD -= delta) < 0)
        p->dashData.dashCD = 0;
    if((p->shData.shootCD -= delta) < 0)
        p->shData.shootCD = 0;

    //let the states do their magic
    if(p->state != NULL)
        p->state(p);
    if(p->movState != NULL)
        p->movState(p);

    //update state_init flag
    if(prevState == p->state)
        p->flags &= ~STATE_INIT;
    if(prevMovState == p->movState)
        p->flags &= ~MOV_STATE_INIT;

    //consume some flags
    p->flags &= ~(ON_THE_GROUND);
    p->keyDown = 0;
}

/**
 * @brief When the player is on the ground and is not moving.
 */
void Player_still(Player *p) {
    if (p->flags & STATE_INIT) {
        Player_setMovState(NO_MOV, p);
    }

    //if the player is still, he shouldn't slide
    p->phObj->velocity.x = 0;

    //could be optimized with else if, but it looks better like this
    //the above applies to all state transition tables
    if(p->contKeyDown & (MOV_LEFT | MOV_RIGHT))
        Player_setState(WALKING, p);
    if(p->keyDown & JUMP_KEY)
        Player_setState(GOING_UP, p);
    if(p->phObj->velocity.y < 0)
        Player_setState(GOING_DOWN, p);
    if(p->keyDown & DASH_KEY)
        Player_setState(DASHING, p);
    if(p->keyDown & ATT_KEY)
        Player_setState(ATTACKING, p);
    if(p->keyDown & SHOOT_KEY)
        Player_setState(SHOOTING, p);
    if(p->flags & DAMAGED)
        Player_setState(DEAD, p);
}

/**
 * @brief When the player is on the ground and is walking.
 */
void Player_walking(Player *p) {
    if (p->flags & STATE_INIT) {
        Player_setMovState(GROUND, p);
    }

    //transisiton table
    //if the keys are not pressed, then go to STILL state
    if(!(p->contKeyDown & MOV_LEFT && !(p->contKeyDown & MOV_RIGHT)) &&
            !(p->contKeyDown & MOV_RIGHT && !(p->contKeyDown & MOV_LEFT)))
        Player_setState(STILL,p);
    if(p->keyDown & JUMP_KEY)
        Player_setState(GOING_UP, p);
    if(p->phObj->velocity.y < 0)
        Player_setState(GOING_DOWN, p);
    if(p->keyDown & DASH_KEY)
        Player_setState(DASHING, p);
    if(p->keyDown & ATT_KEY)
        Player_setState(ATTACKING, p);
    if(p->keyDown & SHOOT_KEY)
        Player_setState(SHOOTING, p);
    if(p->flags & DAMAGED)
        Player_setState(DEAD, p);
}

/**
 * @brief When the player is in the air and is going upwards.
 */
void Player_goingUp(Player *p) {
    if(p->flags & STATE_INIT) {
        //init he state, give the player some Y axis impulse
        p->phObj->velocity.y = JUMP_SPEED;

        Player_setMovState(FLY, p);
    }

    //transition table
    if(p->flags & ON_THE_GROUND) {
        Player_setState(STILL, p);
        if(p->contKeyDown & (MOV_LEFT | MOV_RIGHT))
            Player_setState(WALKING, p);
    }

    if(p->phObj->velocity.y < 0)
        Player_setState(GOING_DOWN, p);
    if(p->keyDown & DASH_KEY)
        Player_setState(DASHING, p);
    if(p->keyDown & ATT_KEY)
        Player_setState(ATTACKING, p);
    if(p->keyDown & SHOOT_KEY)
        Player_setState(SHOOTING, p);
    if(p->flags & DAMAGED)
        Player_setState(DEAD, p);
}

/**
 * @brief When the player is in the air and is going downwards.
 */
void Player_goingDown(Player *p) {
    if (p->flags & STATE_INIT) {
        Player_setMovState(FLY, p);
    }

    //transistion table
    if(p->flags & ON_THE_GROUND) {
        Player_setState(STILL, p);
        if(p->contKeyDown & (MOV_LEFT | MOV_RIGHT))
            Player_setState(WALKING, p);
    }
    if(p->keyDown & DASH_KEY)
        Player_setState(DASHING, p);
    if(p->keyDown & ATT_KEY)
        Player_setState(ATTACKING, p);
    if(p->keyDown & SHOOT_KEY)
        Player_setState(SHOOTING, p);
    if(p->flags & DAMAGED)
        Player_setState(DEAD, p);
}

/**
 * @brief When the player is dead.
 */

void Player_dead(Player *p) {
    if(p->flags & STATE_INIT){
        Player_setMovState(NO_MOV, p);
    }
}
/**
 * @brief When the player is shooting a bulett.
 */
void Player_shoot(Player *p) {
    if(p->flags & STATE_INIT && p->shData.shootCount != 0 && !p->shData.shootCD) {
        uint32_t k = p->contKeyDown;
        int const sh = 10, lo = 20; // short and long dimensions
        float pW, pH, pad;
        pW = p->phObj->aabb.hWidth;
        pH = p->phObj->aabb.hHeight;
        pad = 10;

        //we set the pos to 0,0, the next operation will take care
        //of positioning
        Object *shootBox = NULL;
        int pX = p->phObj->aabb.center.x;
        int pY = p->phObj->aabb.center.y;

        //we create boxes according to where the player is 'facing' currently
        if(k & MOV_UP) {
            shootBox = PH_createBox(pX - (sh/2), pY + (pH + pad), sh, lo, 1, HYBRID, p->world);
            shootBox->velocity.y = BULLET_SPEED;
        } else if (k & MOV_DOWN) {
            shootBox = PH_createBox(pX - (sh/2), pY - (pH + pad + lo), sh, lo, 1, HYBRID, p->world);
            shootBox->velocity.y = -BULLET_SPEED;
        } else if (k & MOV_LEFT) {
            shootBox = PH_createBox(pX - (pW + pad + lo), pY - (sh/2), lo, sh, 1, HYBRID, p->world);
            shootBox->velocity.x = -BULLET_SPEED;
        } else if (k & MOV_RIGHT) {
            shootBox = PH_createBox(pX + (pW + pad), pY - (sh/2), lo, sh, 1, HYBRID, p->world);
            shootBox->velocity.x = BULLET_SPEED;
        }


        //means we have created a shootbox
        if(shootBox != NULL) {
            //we init stuff
            Bag_push(shootBox, p->shData.bag);
            PH_setUData(p, BULLET, shootBox);
            PH_setCallback((PH_callback)&Player_bulletCB, p, shootBox);
            p->shData.shootCD = SHOOT_CD;
            p->shData.shootCount--;
            shootBox->color = p->phObj->color;
        }
    }

    //transitioning
    if (p->flags & ON_THE_GROUND) {
        Player_setState(STILL, p);
        if (p->contKeyDown & (MOV_LEFT | MOV_RIGHT))
            Player_setState(WALKING, p);
        if (p->keyDown & JUMP_KEY && p->flags & ON_THE_GROUND)
            Player_setState(GOING_UP, p);
    }

    if (p->phObj->velocity.y < 0)
        Player_setState(GOING_DOWN, p);
    if(p->keyDown & DASH_KEY)
        Player_setState(DASHING, p);
    if(p->keyDown & ATT_KEY)
        Player_setState(ATTACKING, p);
    if(p->flags & DAMAGED)
        Player_setState(DEAD, p);
}

int Player_bulletCB(PH_Manifold *m, Object *A, Object *B, Player *p) {

    //collision with an attackbox is an exception
    //as it does not destroy the bullet
    if(B->userData.type == ATTACKBOX) {
        A->velocity.x *= -1;
        A->velocity.y *= -1;
        A->cbState = B->userData.data;
        //if the shot already participated in a callback, we ignore further collisions
    } else if(Bag_search(A, destroyBag) == -1) {

        if (B->userData.type == PLAYER) {
            Player *damP = (Player *) B->userData.data;
            damP->flags |= DAMAGED;
            Bag_push(A, destroyBag);
            Bag_unorderedRemove(Bag_search(A, p->shData.bag), p->shData.bag);
            p->score++;
        } else if (B->userData.type == BLOCK) {
            if (Bag_search(B, destroyBag) == -1)
                Bag_push(B, destroyBag);
            Bag_push(A, destroyBag);
            Bag_unorderedRemove(Bag_search(A, p->shData.bag), p->shData.bag);
        } else if (B->userData.type == WALL) {
            if (Bag_search(A, destroyBag) == -1)
                Bag_push(A, destroyBag);
            Bag_unorderedRemove(Bag_search(A, p->shData.bag), p->shData.bag);
        }
    }

    return 0;
}

/**
 * @brief When the player is currently dashing.
 */
void Player_dash(Player *p) {
    if(p->flags & STATE_INIT && !p->dashData.dashCD) {
        uint32_t k = p->contKeyDown;
        p->dashData.dir.x = p->dashData.dir.y = 0;

        if(k & MOV_LEFT) {
            p->dashData.dir.x -= DASH_SPEED;
            p->dashData.isLive = 1;
        } else if(k & MOV_RIGHT) {
            p->dashData.dir.x += DASH_SPEED;
            p->dashData.isLive = 1;
        }

        if(p->dashData.isLive) {
            p->dashData.dashCD = DASH_CD;
            if(p->dashData.dir.x != 0)
                p->phObj->velCapX = DASH_SPEED;
            if(p->dashData.dir.y != 0)
                p->phObj->velCapY = DASH_SPEED;
            TM_new((Timer_callBack)&Player_dashTimer, p);
        }
    }

    //transition only if the dash has been finished
    if(p->dashData.isLive == 0) {
        if(p->flags & ON_THE_GROUND) {
            Player_setState(STILL, p);
            if(p->contKeyDown & (MOV_LEFT | MOV_RIGHT))
                Player_setState(WALKING, p);
        }
        if(p->keyDown & ATT_KEY)
            Player_setState(ATTACKING, p);
        if(p->keyDown & SHOOT_KEY)
            Player_setState(SHOOTING, p);
    }

    if(p->flags & DAMAGED) {
        Player_setState(DEAD, p);
        p->dashData.isLive = 0;
        p->phObj->velCapX = XCAP;
        p->phObj->velCapY = YCAP;
        p->phObj->velocity.x  = p->phObj->velocity.y = 0;
    }
}

int Player_dashTimer(Uint32 delta, Timer *timer, Player *p) {
    //the player has been destroyed
    //the attackbox has been destroyed
    if(p->dashData.isLive == 0)
        return 1;

    Timer_updateDelta(delta, timer);

    Uint32 ticks = Timer_getTicks(timer);
    if(ticks > DASH_DURR) {
        p->dashData.isLive = 0;
        p->phObj->velCapX = XCAP;
        p->phObj->velCapY = YCAP;
        p->phObj->velocity.x  = p->phObj->velocity.y = 0;
        return 1;
    } else {
        p->phObj->velocity = p->dashData.dir;
        p->phObj->forceSum.x = p->phObj->forceSum.y = 0;
        return 0;
    }

}


/**
 * @brief When the player is spawning an attackbox.
 */
void Player_attack(Player *p) {
    if (p->flags & STATE_INIT && !p->attData.attCD) {
        Player_setMovState(NO_MOV, p);

        //here we take care of spawning attack boxes
        //and do so only if there is none
        if(p->attData.isLive == 0) {
            uint32_t k = p->contKeyDown;
            int const sh = 10, lo = 39; // short and long dimensions
            float pW, pH, pad;
            pW = p->phObj->aabb.hWidth;
            pH = p->phObj->aabb.hHeight;
            pad = 10;

            //we set the pos to 0,0, the next operation will take care
            //of positioning
            if(k & MOV_UP) {
                p->attData.box = PH_createBox(0, 0, sh, lo, 1, HYBRID, p->world);
                p->attData.relPos.x = -(sh/2);
                p->attData.relPos.y = (pH + pad);
            } else if (k & MOV_DOWN) {
                p->attData.box = PH_createBox(0, 0, sh, lo, 1, HYBRID, p->world);
                p->attData.relPos.x = -(sh/2);
                p->attData.relPos.y = -(pH + pad + lo);
            } else if (k & MOV_LEFT) {
                p->attData.box = PH_createBox(0, 0, lo, sh, 1, HYBRID, p->world);
                p->attData.relPos.x = -(pW + pad + lo);
                p->attData.relPos.y = -(sh/2);
            } else if (k & MOV_RIGHT) {
                p->attData.box = PH_createBox(0, 0, lo, sh, 1, HYBRID, p->world);
                p->attData.relPos.x = (pW + pad);
                p->attData.relPos.y = -(sh/2);
            }

            //means we have created an attackbox
            if(p->attData.box != NULL) {
                //create a timer that will eventually destroy the box and
                //allows us to transition into other states
                TM_new((Timer_callBack) &Player_attackRemoveTimer, p);
                //set the PH_callback
                PH_setCallback((PH_callback) &Player_attackBoxColl, p, p->attData.box);
                p->attData.attCD = ATTACK_CD;
                p->attData.usedUp = 0;
                p->attData.isLive = 1;
                PH_setUData(p, ATTACKBOX, p->attData.box);
                p->attData.box->color = p->phObj->color;
            }
        }
    }

    if(p->attData.isLive)
        PH_setPosition(VEC2D_add(&p->attData.relPos, &p->phObj->aabb.center), p->attData.box);


    //only transition if the attack has ended
    if(p->attData.isLive == 0) {
        if (p->flags & ON_THE_GROUND) {
            Player_setState(STILL, p);
            if (p->contKeyDown & (MOV_LEFT | MOV_RIGHT))
                Player_setState(WALKING, p);
            if (p->keyDown & JUMP_KEY && p->flags & ON_THE_GROUND)
                Player_setState(GOING_UP, p);
        }

        if (p->phObj->velocity.y < 0)
            Player_setState(GOING_DOWN, p);
        if(p->keyDown & DASH_KEY)
            Player_setState(DASHING, p);
        if(p->keyDown & ATT_KEY)
            Player_setState(ATTACKING, p);
        if(p->keyDown & SHOOT_KEY)
            Player_setState(SHOOTING, p);
    }

    //death is an exception, we have to take care to clean up our mess
    if(p->flags & DAMAGED) {
        Player_setState(DEAD, p);
        PH_destroyObject(p->attData.box);
        p->attData.box = NULL;
        p->attData.isLive = 0;
    }
}

int Player_attackRemoveTimer(Uint32 delta, Timer *timer, Player *p) {
    //the player has been destroyed
    //the attackbox has been destroyed
    if(Player_compState(DEAD, p) || p->attData.isLive == 0)
        return 1;

    Timer_updateDelta(delta, timer);

    //this might stay running after
    if(Timer_getTicks(timer) > ATTACK_DURR) {
        PH_destroyObject(p->attData.box);
        p->attData.box = NULL;
        p->attData.isLive = 0;
        //returning 1, because we want the timer to be deleted
        return 1;
    }

    //we don't want the timer deleted yet
    return 0;
}

int Player_attackBoxColl(PH_Manifold *m, Object *A, Object *B, Player *p) {

    if(B->userData.type == PLAYER) {
        ((Player*)B->userData.data)->flags |= DAMAGED;
        p->score++;
    } else if (B->userData.type == BLOCK) {
        if(Bag_search(B, destroyBag) == -1 && !p->attData.usedUp) {
            Bag_push(B, destroyBag);
            p->attData.usedUp = 1;
        }
    } else if (B->userData.type == ATTACKBOX) {
        if(p->attData.relPos.x < 0) {
            Vector2D vec = {CLING_IMPULSE, 0};
            PH_impulse(&vec, p->phObj);
        } else {
            Vector2D vec = {-CLING_IMPULSE, 0};
            PH_impulse(&vec, p->phObj);
        }
    }

    //we never want our attack boxes to collide with stuff
    return 0;
}


//movement states
/**
 * @brief Movement state, when the player is in the air.
 */
void Player_flyMov(Player *p) {
    Vector2D vec = {0, 0};
    AABB aabb = p->phObj->aabb;
    vec.y = aabb.center.y - aabb.hHeight / 2;

    int walljump = 0;

    if (p->keyDown & JUMP_KEY) {
        vec.x = aabb.center.x + aabb.hWidth + 2;
        PH_queryPoint(vec, STATIC | HYBRID, 1, queryBag, p->world);
        if (queryBag->elemCount != 0) {
            p->phObj->velocity.x = -XCAP;
            p->phObj->velocity.y = JUMP_SPEED;
            walljump = 1;
        } else {
            vec.x = aabb.center.x - aabb.hWidth - 2;
            PH_queryPoint(vec, STATIC | HYBRID, 1, queryBag, p->world);
            if (queryBag->elemCount != 0) {
                p->phObj->velocity.x = XCAP;
                p->phObj->velocity.y = JUMP_SPEED;
                walljump = 1;
            }
        }
    }

    if (!walljump) {
        float *velY = &p->phObj->velocity.y;
        if (p->contKeyDown & MOV_LEFT && !(p->contKeyDown & MOV_RIGHT)) {
            vec.x = aabb.center.x - aabb.hWidth - 2;
            PH_queryPoint(vec, STATIC | HYBRID, 1, queryBag, p->world);
            if(queryBag->elemCount != 0) {
                *velY = *velY < -SLIDE_MAX ? -SLIDE_MAX : *velY;
            } else {
                vec.x = -FLY_FORCE;
                vec.y = 0;
                PH_force(&vec, p->phObj);
            }
        } else if (p->contKeyDown & MOV_RIGHT && !(p->contKeyDown & MOV_LEFT)) {
            vec.x = aabb.center.x + aabb.hWidth + 2;
            PH_queryPoint(vec, STATIC | HYBRID, 1, queryBag, p->world);
            if(queryBag->elemCount != 0) {
                *velY = *velY < -SLIDE_MAX ? -SLIDE_MAX : *velY;
            } else {
                vec.x = FLY_FORCE;
                vec.y = 0;
                PH_force(&vec, p->phObj);
            }
        }
    }
}

/**
 * @brief Movement when the player is on the ground.
 */
void Player_groundMov(Player *p) {
    Vector2D vec = {0, 0};

    if(p->contKeyDown & MOV_LEFT && !(p->contKeyDown & MOV_RIGHT)) {
        vec.x = -WALK_FORCE;
        PH_force(&vec, p->phObj);
    } else if(p->contKeyDown & MOV_RIGHT && !(p->contKeyDown & MOV_LEFT)) {
        vec.x = WALK_FORCE;
        PH_force(&vec, p->phObj);
    }
}














