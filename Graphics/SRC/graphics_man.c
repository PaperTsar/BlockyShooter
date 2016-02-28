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

#include <SDL2/SDL_image.h>
#include "SDL2/SDL_ttf.h"
#include "../HEAD/textsprite.h"


/**
 * @brief Window width.
 */
const int SCREEN_WIDTH = 1200;

/**
 * @brief Window height.
 */
const int SCREEN_HEIGHT = 700;


/**
 * @brief Pointer to the single window this project has.
 */
SDL_Window *gWindow = NULL;
/**
 * @brief Pointer the single render object this project has.
 */
SDL_Renderer *gRenderer = NULL;

/**
 * @brief Initializes the ttf, image and core sdl library, creates a window.
 * @param SDL_FLAGS the subsystems the SDL library should init.
 * @param IMG_FLAGS the subsystems the IMG library should init.
 */
int GM_init(int SDL_FlAGS, int IMG_FLAGS) {
    //initializing video system
    if (SDL_Init(SDL_FlAGS) < 0)
        goto error_videoSystem;

    //this don't work on me Ubuntu :'( sad face
    //dem nvidia drivers won't work, me too nub for linux to set them up correctly
    SDL_GL_SetSwapInterval(1);
    //creating a window
    if ((gWindow = SDL_CreateWindow("Tucsok",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SCREEN_WIDTH,
                                    SCREEN_HEIGHT,
                                    SDL_WINDOW_SHOWN)) == NULL)
        goto error_createWindow;

    //creating the renderer
    if ((gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL)
        goto error_createRenderer;

    //maybe it will work if I include this here too, except dat it don't want to :'( sad face
    SDL_GL_SetSwapInterval(1);

    //set the initial rendering draw color to white
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    //initialize the IMG library
    if (IMG_Init(IMG_FLAGS) != IMG_FLAGS)
        goto error_initIMG;

    //initialize the TTF libarary
    if (TTF_Init() != 0)
        goto error_initTTF;

    return 0;
    //          [END OF NORMAL CONTROL FLOW]

    //          [ERROR HANDLING]
    error_initTTF:
    printf("FUNC: init_GM. Error initializing TTF subsystem. TTF_ERROR: %c.\n", TTF_GetError);
    error_videoSystem:
    printf("FUNC: init_GM. Error initializing video system. SDL_ERROR: %s.\n", SDL_GetError());
    return -1;
    error_createWindow:
    printf("FUNC: init_GM. Error creating window. SDL_ERROR: %s.\n", SDL_GetError());
    return -1;
    error_createRenderer:
    printf("FUNC: init_GM. Error creating renderer. SDL_ERROR: %s.\n", SDL_GetError());
    return -1;
    error_initIMG:
    printf("FUNC: init_GM. Error initializing SDL_image. IMG_ERROR %s.\n", IMG_GetError());
    return -1;
}

/**
 * @brief Destroys everything GM_init() has initialized.
 */
void GM_deinit() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    gRenderer = NULL;
    gWindow = NULL;

    IMG_Quit();
    SDL_Quit();
}
/**
 * @brief Loads a png image from a file and creates an SDL_Texture from it.
 *
 * Takes 3 colour variables {r,g,b} to set the color keying of the SDL_texture.
 * If either of them is negative, no colorkeying will be set.
 */
SDL_Texture *GM_loadPngFromFile(char *path, int r, int g, int b) {
    SDL_Texture *finalTexture = NULL;
    SDL_Surface *tmpSurface = NULL;

    //create a surface from the png file
    if ((tmpSurface = IMG_Load(path)) == NULL)
        goto error_getSurfaceFromFile;

    //set the colorkeying if one was specified
    if (r >= 0 && g >= 0 && b >= 0)
        SDL_SetColorKey(tmpSurface, SDL_TRUE, SDL_MapRGB(tmpSurface->format, r, g, b));

    //createa texture
    if ((finalTexture = SDL_CreateTextureFromSurface(gRenderer, tmpSurface)) == NULL)
        goto error_createTexture;

    //free the surface, we have no need for it
    SDL_FreeSurface(tmpSurface);
    return finalTexture;
    //          [END OF NORMAL CONTROL FLOW]

    //          [ERROR HANDLING]
    error_createTexture:
    printf("FUNC: loadFromFile_GM. Path: %s. Error creating texture. IMG_ERROR: %s.\n", path, SDL_GetError());
    SDL_FreeSurface(tmpSurface);
    return NULL;
    error_getSurfaceFromFile:
    printf("FUNC: loadFromFile_GM. Path: %s. Error loading Surface from file. SDL_ERROR: %s.\n", path, IMG_GetError());
    return NULL;
}
