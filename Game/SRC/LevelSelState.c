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

#include "../HEAD/LevelSelState.h"
#include "../../Graphics/HEAD/textsprite.h"
#include "../../Graphics/HEAD/graphics_man.h"
#include "../../Events/HEAD/input.h"
#include "../HEAD/main.h"

/*
 * This is basically a copy of the Main_menu state, but for levelselect.
 * This has t same exact mechanics as the Main_menu state.
 */

typedef enum LevelSel_Options {
    LOWER_BOUND = -1,
    MAP1,
    MAP2,
    MAP3,
    MAP4,
    MAP5,
    UPPER_BOUND
} LevelSel_Options;

typedef struct LevelSel_data {
    int enterDown;
    int escDown;
    LevelSel_Options currSel;
} LevelSel_data;

TextSprite *LevelSel_selected[UPPER_BOUND];
TextSprite *LevelSel_unselected[UPPER_BOUND];

SDL_Texture *levelSelBackground = NULL;
LevelSel_data levelSel;

int LevelSelInputProc(SDL_Event *e, void *null);

int LevelSel_start() {
    int i; //generic iterator
    levelSel.currSel = MAP1;
    levelSel.enterDown = 0;
    levelSel.escDown = 0;

    //loading assets
    if( (levelSelBackground = GM_loadPngFromFile("res/Menu/background.png", -1, -1, -1)) == NULL)
        return -1;

    char *Menu_texts[UPPER_BOUND] = {"LEVEL 1", "LEVEL 2", "LEVEL 3", "LEVEL 4", "LEVEL 5"};
    SDL_Color unselectedColor = {0x88, 0x94, 0xAE, 0xFF};
    SDL_Color   selectedColor = {0xAB, 0xE5, 0xF3, 0xFF};
    const int TEXTSIZE = 70;

    //creating the textSprites
    for(i = 0; i<UPPER_BOUND; i++) {
        int w, h;
        TextSprite *curr = TS_new();
        if(TS_setText(Menu_texts[i], &selectedColor, TEXTSIZE, curr))
            return -1;
        w = SCREEN_WIDTH/2 - TS_getWidth(curr)/2;
        h = SCREEN_HEIGHT - (SCREEN_HEIGHT - (TS_getHeight(curr)*UPPER_BOUND) - (UPPER_BOUND-1)*40)/2 - (i+1)*TS_getHeight(curr) - i*40;
        TS_setPos(w, h, curr);
        LevelSel_selected[i] = curr;

        //unselected
        curr = TS_new();
        if(TS_setText(Menu_texts[i], &unselectedColor, TEXTSIZE, curr))
            return -1;
        TS_setPos(w, h, curr);
        LevelSel_unselected[i] = curr;
    }

    Input_subscribe((inputConsumer)&LevelSelInputProc, NULL);

    return 0;
}

void LevelSel_func(uint32_t delta) {
    int i; //generic iterator


    //Rendering
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);

    SDL_RenderCopy(gRenderer, levelSelBackground, NULL, NULL);
    for(i=0; i<UPPER_BOUND; i++)
        if(i == levelSel.currSel)
            TS_render(LevelSel_selected[i]);
        else
            TS_render(LevelSel_unselected[i]);


    SDL_RenderPresent(gRenderer);

    if(levelSel.escDown) {
        SwapGlobalState(MAIN_MENU);
        return;
    }

    if(levelSel.enterDown) {
        switch(levelSel.currSel) {
            case MAP1:
                currMapPath = "res/maps/map1.dat";
                break;
            case MAP2:
                currMapPath = "res/maps/map2.dat";
                break;
            case MAP3:
                currMapPath = "res/maps/map3.dat";
                break;
            case MAP4:
                currMapPath = "res/maps/map4.dat";
                break;
            case MAP5:
                currMapPath = "res/maps/map5.dat";
                break;
        }
        levelSel.enterDown = 0;
        SwapGlobalState(MAIN_MENU);
    }
}
int LevelSel_end() {
    int i; //generic iterator
    SDL_DestroyTexture(levelSelBackground);
    for(i = 0; i<UPPER_BOUND; i++) {
        TS_free(LevelSel_selected[i]);
        TS_free(LevelSel_unselected[i]);
    }
    return 0;
}


int LevelSelInputProc(SDL_Event *e, void *null) {
    int consumed = 1;
    if(e->type == SDL_KEYDOWN && !e->key.repeat) {
        switch(e->key.keysym.sym) {
            case SDLK_UP:
                if(--(levelSel.currSel) <= LOWER_BOUND)
                    levelSel.currSel = UPPER_BOUND - 1;
                break;
            case SDLK_DOWN:
                if(++(levelSel.currSel) >= UPPER_BOUND)
                    levelSel.currSel = 0;
                break;
            case SDLK_RETURN:
                levelSel.enterDown = 1;
                break;
            case SDLK_ESCAPE:
                levelSel.escDown = 1;
                break;
            default:
                consumed = 0;
                break;
        }
    }

    return consumed;
}
