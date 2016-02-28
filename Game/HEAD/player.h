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

#ifndef DUMMY_PLAYER_H
#define DUMMY_PLAYER_H


#include "../../Collision/HEAD/physics.h"
#include "../HEAD/player.h"


/**
 * @file
 * @brief Module for handling player related stuff.
 * @author Bendegúz Nagy
 *
 * The player module uses two concurrent function table FSMs. One for movement and one for logic. State init is done
 * with the STATE_INIT flag in state functions. Movement state init is done with MOV_STATE_INIT flag in the movement
 * state functions.
 *
 * Each state function follows the same pattern. Check the STATE_INIT flag, do some logic, check if transition should
 * occur to another state.
 */


typedef struct Player Player;

/**
 * @brief Enumeration of not continuous player keys, stored in an int by OR-ing together.
 */
typedef enum PLAYER_KEYFLAGS {
    ATT_KEY = 1,
    JUMP_KEY = 2,
    DASH_KEY = 4,
    SHOOT_KEY = 8
} PLAYER_KEYFLAGS;

/**
 * @brief Enumeration of continuous player keys, stored in an int by OR-ing together.
 */
typedef enum PLAYER_CONTKEYFLAGS {
    MOV_UP = 1,
    MOV_DOWN = 2,
    MOV_LEFT = 4,
    MOV_RIGHT = 8
} PLAYER_CONTKEYFLAGS;


/**
 * @brief Player state flags, stored in an int by OR-ing together.
 */
typedef enum PLAYER_FLAGS {
    //set by the player collision callback
    ON_THE_GROUND = 1,
    //means the player is gonna be dead
    DAMAGED = 2,
    //used for initing states
    STATE_INIT = 4,
    //used for initing movStates
    MOV_STATE_INIT = 8
} PLAYER_FLAGS;

/**
 * @brief The player can only be a single state, namely one of these.
 */
typedef enum PLAYER_STATE {
    STILL = 0,
    WALKING = 1,
    GOING_UP = 2,
    GOING_DOWN = 3,
    ATTACKING = 4,
    DEAD = 5,
    DASHING = 6,
    SHOOTING = 7
} PLAYER_STATE;

/**
 * @brief The player can only be in one of these movement states.
 */
typedef enum PLAYER_MOV_STATE {
    NO_MOV = 0, //STILL, DEAD
    FLY = 1, //GOING_UP, GOING_DOWN,
    GROUND = 2 //WALKING
} PLAYER_MOV_STATE;

/**
 * @brief Movement and logic state function must adhere to this function.
 */
typedef void (*stateFunc)(Player *p);

/**
 * @brief Each player has one of these, hold data for the attacking state.
 */
typedef struct AttackData {
    int isLive; //is the player currently attacking?
    int usedUp; //has e used it up to destory a block?
    Vector2D relPos; //realitve poisiton of the attackbox in regards to the player
    Object *box; //the attackbox object thing
    int attCD; //cooldown of the attack
} AttackData;

/**
 * @brief Each player has one of these, hold data for the dashing state.
 */
typedef struct DashData {
    int dashCD; //dash cooldown
    int isLive; //is this player in dash?
    Vector2D dir; //direction of the dash
} DashData;

/**
 * @brief Each player has one of these, hold data for the shooting state.
 */
typedef struct ShootData {
    int shootCD; //cooldown for shooting
    int shootCount; //nmber of bullets left
    Bag *bag; //bag containing the bullets of this player
} ShootData;

/**
 * @brief Holds every data defining a player.
 */
typedef struct Player {
    //the world this player belongs to
    World *world;
    //the physics object that represents the player
    Object *phObj;
    //up, down, left, right, attack, jump, dash, shoot
    SDL_Keycode keys[8];

    //current state
    stateFunc state;
    //current mevement state
    stateFunc movState;

    //continuous key include keys like wasd
    uint32_t contKeyDown;
    //trigger keys like attack and jump
    uint32_t keyDown;
    //player flags
    uint32_t flags;

    //the score of this curent player
    int score;

    //attacking state data
    AttackData attData;
    //dashing state data
    DashData dashData;

    //shooting state data
    ShootData shData;
} Player;

void Player_initModule();
void Player_deinitModule();

Player *Player_new(int x, int y, World *world);
void Player_free(Player *player);
void Player_reset(Player *p);

int Player_feedInput(SDL_Event *e, Player *p);
void Player_update(Player *p, Uint32 delta);
void Player_postRender(Uint32 delta);

void Player_setState(PLAYER_STATE state, Player *p);
int Player_compState(PLAYER_STATE state, Player *p);

void Player_setMovState(PLAYER_MOV_STATE state, Player *p);

void Player_setControl(SDL_Keycode up, SDL_Keycode down, SDL_Keycode left, SDL_Keycode right,
                       SDL_Keycode attack, SDL_Keycode jump, SDL_Keycode dash, SDL_Keycode shoot, Player *player);


#endif //DUMMY_PLAYER_H
