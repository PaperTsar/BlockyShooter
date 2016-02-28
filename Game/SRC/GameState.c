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

#include "../HEAD/GameState.h"
#include "../../Collision/HEAD/physics.h"
#include "../HEAD/player.h"
#include "../HEAD/main.h"
#include "../../Events/HEAD/input.h"
#include "../../Events/HEAD/timer.h"
#include "../../Events/HEAD/Timer_man.h"
#include "../../Graphics/HEAD/graphics_man.h"
#include "../../Graphics/HEAD/textsprite.h"

#define PLAYER_COUNT 2
#define GRAVITY -1700
#define RESPAWN_TIME 1000
#define WIN_SCORE 5

/**@brief The physics world singleton used for the game.*/
World *world;
/**@brief Array holding player objects, currently hardcoded for 2.*/
Player *players[PLAYER_COUNT];
/**@brief Bag holding 2D vectors of possible spawn positions, read from map file.*/
Bag *spawnPos = NULL;

/**@brief Flag for "is game paused?" question.*/
int Game_paused;
/**@brief End game picture.*/
SDL_Texture *youreWinner = NULL;
/**@brief Rectangle for rendering end game picture.*/
SDL_Rect youreWinnerRect = {490, 200, 221, 237};
/**@brief Text object to be displayed at the end of a game.*/
TextSprite *winText = NULL;

/**@brief Respawns a player.*/
int Game_respawn(uint32_t delta, Timer *timer, Player *p);

/**@brief Input consumer used at the end of a game to process the ESC key.*/
int Game_escapeInputProc(SDL_Event *e, void *null);

int Game_start()
{
    int i;

    //load assets
    Game_paused = 0;
    youreWinner = GM_loadPngFromFile("res/Game/yourewinner.png", -1, -1, -1);
    if (!youreWinner)
        return -1;
    winText = TS_new();
    SDL_Color c = {0x02, 0x47, 0x00, 0xFF};
    TS_setText("Great, now press ESC and go do something productive.", &c, 30, winText);
    TS_setPos(SCREEN_WIDTH / 2 - TS_getWidth(winText) / 2, 200, winText);


    //load map
    spawnPos = Bag_new((freeData) &VEC2D_free);
    world = PH_createWorld();
    PH_setGravity(0, GRAVITY, world);
    PH_setStepTime(1.0 / 120.0, world);

    FILE *file = fopen(currMapPath, "rt");
    if (!file)
        return -1;
    int doneReadingMapFile = 0;

    while (!doneReadingMapFile) {
        int v[5];
        for (i = 0; i < 5; i++) {
            int scanfRet_value = fscanf(file, "%d", v + i);
            if(scanfRet_value == EOF)
                doneReadingMapFile = 1;
            else if(scanfRet_value != 1) {
                fclose(file);
                return -1;
            }
        }

        if (!doneReadingMapFile)
            switch (v[0]) {
                case PLAYER: {
                    Vector2D *vec = (Vector2D *) malloc(sizeof(Vector2D));
                    vec->x = v[1];
                    vec->y = v[2];
                    Bag_push(vec, spawnPos);
                    break;
                }
                case BLOCK: {
                    Object *o = PH_createBox(v[1], v[2], v[3], v[4], 1, HYBRID, world);
                    PH_setUData(NULL, BLOCK, o);
                    PH_setColor(200, 200, 200, 0xFF, o);
                    break;
                }
                case WALL: {
                    Object *o = PH_createBox(v[1], v[2], v[3], v[4], 1, HYBRID, world);
                    PH_setUData(NULL, WALL, o);
                    PH_setColor(100, 100, 100, 0xFF, o);
                    break;
                }
            }

    }

    fclose(file);
    //means we have less than two spawnpoints
    if(spawnPos->elemCount < 2)
        return -1;

    //spawn players and set them up
    Vector2D *s1 = spawnPos->vector[rand() % spawnPos->elemCount];
    Vector2D *s2 = s1;
    while (s2 == s1)
        s2 = spawnPos->vector[rand() % spawnPos->elemCount];

    Player_initModule();
    players[0] = Player_new(s1->x, s1->y, world);
    players[1] = Player_new(s2->x, s2->y, world);

    PH_setColor(0x43, 0xA1, 0x6F, 0xFF, players[0]->phObj);
    PH_setColor(0xFF, 0x66, 0x00, 0xFF, players[1]->phObj);

    Player_setControl(SDLK_w, SDLK_s, SDLK_a, SDLK_d, SDLK_v, SDLK_f, SDLK_b, SDLK_c, players[0]);
    Player_setControl(SDLK_KP_5, SDLK_KP_2, SDLK_KP_1, SDLK_KP_3, SDLK_DOWN, SDLK_UP, SDLK_RIGHT, SDLK_LEFT,
                      players[1]);

    for (i = 0; i < PLAYER_COUNT; i++)
        Input_subscribe((inputConsumer) &Player_feedInput, players[i]);

    return 0;
}


void Game_func(uint32_t delta)
{
    int i;

    if (Game_paused)
        return;

    //check if someone has reached the winning score
    int winner = 0;
    for (i = 0; i < PLAYER_COUNT; i++)
        if (players[i]->score >= WIN_SCORE) {
            winner = 1;
            break;
        }

    //if there was a winner, 'pause' the game and wait for an esc key
    if (winner) {
        Game_paused = 1;
        SDL_RenderCopy(gRenderer, youreWinner, NULL, &youreWinnerRect);
        TS_render(winText);
        SDL_RenderPresent(gRenderer);
        Input_subscribe((inputConsumer) &Game_escapeInputProc, NULL);

        return;
    } else
        //if there was no winner, then check for dead players, if there are, then pause the game
        //then when the timer is up, resume it
        for (i = 0; i < PLAYER_COUNT; i++)
            if (Player_compState(DEAD, players[i])) {
                Game_paused = 1;
                TM_new((Timer_callBack) &Game_respawn, players[i]);
                break;
            }

    //update the world
    PH_stepWorld(delta / 1000.0, world);

    //this is after
    for (i = 0; i < PLAYER_COUNT; i++)
        Player_update(players[i], delta);


    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);
    //begin render

    //render physics engine
    PH_renderObjects(world);


    //render scores
    SDL_SetRenderDrawColor(gRenderer, 0x43, 0xA1, 0x6F, 0xFF);
    for (i = 0; i < players[0]->score; i++) {
        SDL_Rect r = {50 + i * 30, 15, 15, 15};
        SDL_RenderFillRect(gRenderer, &r);
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x66, 0x00, 0xFF);
    for (i = 0; i < players[1]->score; i++) {
        SDL_Rect r = {1150 - i * 30, 665, 15, 15};
        SDL_RenderFillRect(gRenderer, &r);
    }

    //end render
    SDL_RenderPresent(gRenderer);

    //post render stuff
    Player_postRender(delta);
}

int Game_end()
{
    Bag_free(spawnPos, 1);
    int i;
    for (i = 0; i < PLAYER_COUNT; i++)
        Player_free(players[i]);

    PH_destroyWorld(world);

    SDL_DestroyTexture(youreWinner);
    TS_free(winText);
    Player_deinitModule();
    return 0;
}


//private methods


int Game_respawn(uint32_t delta, Timer *timer, Player *p)
{
    Timer_updateDelta(delta, timer);

    //if the timer is up then choose a random respawn for the player
    //and resume the game
    if (Timer_getTicks(timer) >= RESPAWN_TIME) {
        Vector2D *vec = spawnPos->vector[rand() % spawnPos->elemCount];
        PH_setPosition(*vec, p->phObj);
        Player_reset(p);
        Game_paused = 0;
        return 1;
    }

    return 0;
}

int Game_escapeInputProc(SDL_Event *e, void *null)
{
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_ESCAPE) {
            SwapGlobalState(MAIN_MENU);
            return 1;
        }
    }
    return 0;
}
