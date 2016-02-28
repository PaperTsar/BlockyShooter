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

#include <stdio.h>
#include <SDL_image.h>
#include <time.h>
#include "../HEAD/main.h"
#include "../HEAD/MenuState.h"
#include "../../Graphics/HEAD/graphics_man.h"
#include "../../Events/HEAD/timer.h"
#include "../../Events/HEAD/Timer_man.h"
#include "../../Events/HEAD/input.h"
#include "../HEAD/LevelSelState.h"
#include "../HEAD/GameState.h"
#include "../../Graphics/HEAD/textsprite.h"



/**
 * @brief Main specific data.
 */
typedef struct MAIN_DATA {
    /**@brief The exit flag.*/
    int keepRunning;
    /**@brief The the passed since the last main cycle run.*/
    uint32_t delta;
} MAIN_DATA;

/**
 * @brief Singleton for main specific data.
 */
MAIN_DATA mData;

/**@brief Table holding the init functions.*/
StateStart stStart[GLOBAL_STATE_TOTAL];
/**@brief Table holding the flow functions.*/
StateFunc  stFunc[GLOBAL_STATE_TOTAL];
/**@brief Table holding the deinit functions.*/
StateEnd  stEnd[GLOBAL_STATE_TOTAL];

/**@brief Current state.*/
GlobalState currState = MAIN_MENU;
char *currMapPath = "res/maps/map1.dat";

int Main_init();
void Main_deinit();
/**@brief Input consumer for Window X clicks.*/
int Main_quitInputCB(SDL_Event *e, void *null);

int main(  int argc, char* args[] ) {
    //set things up, if stuff happens, then Main_init will return non-zero
    //in that case we exit the app
    if(Main_init()) {
        Main_deinit();
        return -1;
    }


    //this is our main loop
    while(mData.keepRunning) {
        //get delta
        mData.delta = getDelta();

        //update the timers
        TM_process(mData.delta);
        //consume the accumulated input events
        Input_process();
        //call the current state function
        stFunc[currState](mData.delta);
    }


    //exit normally
    Main_deinit();
    return 0;
}

void SwapGlobalState(GlobalState stateTo) {
    //call the end function of the current state
    if(stEnd[currState]())
        //if it fails, the set the exit flag
        Main_setExitFlag();

    //set the new state
    currState = stateTo;

    //destroy all the timers and input consumers
    TM_clear();
    Input_clear();
    //register our exit input processor
    Input_subscribe((inputConsumer)&Main_quitInputCB, NULL);

    //call the start function of the new state
    if(stStart[currState]())
        Main_setExitFlag();
}

int Main_init() {
    //init random numbers
    srand(time(NULL));
    //the state we will start from
    GlobalState initState = MAIN_MENU;

    //initializes MAINDATA
    mData.keepRunning = 1;
    mData.delta = 0;


    //setup state table
    stStart[MAIN_MENU] = &Menu_start;
    stFunc[MAIN_MENU] = &Menu_func;
    stEnd[MAIN_MENU] = &Menu_end;
    stStart[LEVEL_SELECT_MENU] = &LevelSel_start;
    stFunc[LEVEL_SELECT_MENU] = &LevelSel_func;
    stEnd[LEVEL_SELECT_MENU] = &LevelSel_end;
    stStart[GAME] = &Game_start;
    stFunc[GAME] = &Game_func;
    stEnd[GAME] = &Game_end;


    //Start up SDL and create window
    if( GM_init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS, IMG_INIT_PNG) )
        return -1;

    //init modules
    TS_init("res/oblivious.ttf");
    TM_init();
    Input_init();

    Input_subscribe((inputConsumer)&Main_quitInputCB, NULL);
    //init the beginning state
    if( stStart[currState]() )
        return -1;

    return 0;
}

int Main_quitInputCB(SDL_Event *e, void *null) {
    if(e->type == SDL_QUIT) {
        Main_setExitFlag();
        return 1;
    }

    return 0;
}

void Main_setExitFlag() {
    mData.keepRunning = 0;
}

void Main_deinit() {
    stEnd[currState]();
    Input_deinit();
    TM_deinit();
    TS_deinit();
    GM_deinit();
}
