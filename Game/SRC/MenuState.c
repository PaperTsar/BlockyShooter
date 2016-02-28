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

#include <SDL_render.h>
#include "../HEAD/MenuState.h"
#include "../../Graphics/HEAD/graphics_man.h"
#include "../../Graphics/HEAD/textsprite.h"
#include "../../Events/HEAD/input.h"
#include "../HEAD/main.h"


/**
 * @brief Possible menu options.
 */
typedef enum Menu_options {
    /**@brief Used to identify loop around in menu select.*/
    LOWER_BOUND = -1,
    START_GAME,
    SELECT_LEVEL,
    EXIT,
    /**@brief Used to identify loop around in menu select.*/
    UPPER_BOUND
} Menu_options;

/**
 * @brief Main menu specific data.
 */
typedef struct Menu_data {
    int enterDown;
    /**@brief Currently selected menu option.*/
    Menu_options currSel;
} Menu_data;

/**@brief Array holding text objects of menu options in selected colour.*/
TextSprite *Menu_selected[UPPER_BOUND];
/**@biref Array holding text objects of menu options in unselected colour.*/
TextSprite *Menu_unselected[UPPER_BOUND];

/**@brief Menu background picture.*/
SDL_Texture *menuBackground = NULL;
/**@brief Singleton for Menu specific data.*/
Menu_data menuData;

/**@brief Input consumer for the menu, manages up, down arrows and the enter key.*/
int MenuInputProc(SDL_Event *e, void *null);

int Menu_start(void) {
    int i; //generic iterator
    menuData.currSel = START_GAME;
    menuData.enterDown = 0;

    /*
     *This part is boring, all it does is load assets
     */
    if( (menuBackground = GM_loadPngFromFile("res/Menu/background.png", -1, -1, -1)) == NULL)
        return -1;

    char *Menu_texts[UPPER_BOUND] = {"START GAME", "LEVEL SELECT", "EXIT"};
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
        Menu_selected[i] = curr;

        //unselected
        curr = TS_new();
        if(TS_setText(Menu_texts[i], &unselectedColor, TEXTSIZE, curr))
            return -1;
        TS_setPos(w, h, curr);
        Menu_unselected[i] = curr;
    }

    Input_subscribe((inputConsumer)&MenuInputProc, NULL);

    return 0;
}

void Menu_func(uint32_t delta) {
    int i; //generic iterator


    //clear the screen
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);

    //render background
    SDL_RenderCopy(gRenderer, menuBackground, NULL, NULL);
    for(i=0; i<UPPER_BOUND; i++)
        if(i == menuData.currSel)
            TS_render(Menu_selected[i]);
        else
            TS_render(Menu_unselected[i]);


    //present our pretty picture
    SDL_RenderPresent(gRenderer);


    //if the enter key has been pressed, then switch state according to the selectd
    //menu option
    if(menuData.enterDown) {
        switch(menuData.currSel) {
            case START_GAME:
                SwapGlobalState(GAME);
                break;
            case SELECT_LEVEL:
                SwapGlobalState(LEVEL_SELECT_MENU);
                break;
            case EXIT:
                Main_setExitFlag();
                break;
        }
        menuData.enterDown = 0;
    }
}

int Menu_end(void) {
    int i; //generic iterator
    SDL_DestroyTexture(menuBackground);
    for(i=0; i<UPPER_BOUND; i++) {
        TS_free(Menu_selected[i]);
        TS_free(Menu_unselected[i]);
    }
    return 0;
}


// PRIVATE METHODS

int MenuInputProc(SDL_Event *e, void *null) {
    /*
     * This works by cycling on the enumeration values, wrapping is implemented by the upper and lower bound.
     */
    int consumed = 1;
    if(e->type == SDL_KEYDOWN && !e->key.repeat) {
        switch(e->key.keysym.sym) {
            case SDLK_UP:
                if(--(menuData.currSel) <= LOWER_BOUND)
                    menuData.currSel = UPPER_BOUND - 1;
                break;
            case SDLK_DOWN:
                if(++(menuData.currSel) >= UPPER_BOUND)
                    menuData.currSel = 0;
                break;
            case SDLK_RETURN:
                menuData.enterDown = 1;
                break;
            default:
                consumed = 0;
                break;
        }
    }

    return consumed;
}
