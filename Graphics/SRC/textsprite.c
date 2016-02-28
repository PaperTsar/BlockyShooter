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

#include "../HEAD/textsprite.h"
#include "../HEAD/graphics_man.h"
#include "../../Utility/HEAD/bag.h"

/**
 * @brief Internal representation of the global font in a given size.
 *
 * These are made by means of lazy initialization.
 */
typedef struct Fonts {
    int size;
    TTF_Font *font;
} Fonts;

/**
 * @brief Internal representation of a text object.
 */
typedef struct TextSprite {
    SDL_Texture *textTure; //lol:D
    SDL_Rect dest; //this is used for rendering
} TextSprite;


/**
 * @brief Holds lazily initialized Fonts objects.
 */
Bag* fontsBag = NULL;

/**
 * @brief Holds the file path of the global font.
 */
char *fontsPath = NULL;

/**
 * @brief deallocates a Fonts type allocated by TS_new().
 */
void TS_freeFont(Fonts *font) {
    if(font != NULL) {
        if (font->font != NULL)
            TTF_CloseFont(font->font);
        free(font);
    }
}

/**
 * @brief Initializes the module.
 *
 * Initializes the module, this has to be called before the module is put to use. Calling this more than once, without
 * calling TS_deinit in between will cause a memory leak.
 */
void TS_init(char *fontPath) {
    fontsBag = Bag_new((freeData)&TS_freeFont);
    fontsPath = fontPath;
}

/**
 * @brief Deinitializes the module.
 */
void TS_deinit() {
    Bag_free(fontsBag, 1);
    fontsPath = NULL;
}

/**
 * @brief Creates a new empty text object.
 * @return the newly allocated text object.
 */
TextSprite *TS_new() {
    TextSprite *ptr = (TextSprite *) malloc(sizeof(TextSprite));
    ptr->textTure = NULL;
    ptr->dest.x = ptr->dest.y = 0;
    return ptr;
}

/**
 * @brief Deallocates a text object allocated by TS_new().
 */
void TS_free(TextSprite *ptr) {
    if (ptr == NULL)
        return;

    if (ptr->textTure != NULL)
        SDL_DestroyTexture(ptr->textTure);

    free(ptr);
}

/**
 * @brief Set the text of the text object with a given colour and size.
 *
 * This set the text of the text object by rendering an SDL_Texture.
 */
int TS_setText(char *text, SDL_Color *color, int size, TextSprite *ptr) {
    //lazy initialization, if the global font has already been created with
    //the requested font size, it will be used, otherwise one will be created
    TTF_Font *font = NULL;
    int i;
    for(i = 0; i<fontsBag->elemCount; i++) {
        Fonts *f = (Fonts*)(fontsBag->vector[i]);
        if(f->size == size) {
            font = f->font;
            break;
        }
    }

    //means we haven't initialized the font with the given size yet
    if(font == NULL)   {
        Fonts *f = (Fonts*)malloc(sizeof(Fonts));
        f->size = size;
        //here we create the font with the req. size
        if((f->font = TTF_OpenFont(fontsPath, size)) == NULL) {
            TS_freeFont(f);
            goto error_creatingFont;
        }

        Bag_push(f, fontsBag);
        font = f->font;
    }

    //if the text for this TextSprite has been set previously, that means we
    //have to deallocate the allocated texture first
    if (ptr->textTure != NULL)
        SDL_DestroyTexture(ptr->textTure);

    //create the surface with oru rendered text
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, *color);

    if (textSurface == NULL)
        goto error_creatingSurface;

    //convert it to a texture
    if ((ptr->textTure = SDL_CreateTextureFromSurface(gRenderer, textSurface)) == NULL)
        goto error_creatingTexture;

    //set the rendering rect's properties
    SDL_QueryTexture(ptr->textTure, NULL, NULL, &ptr->dest.w, &ptr->dest.h);

    //free the surface (we created a texture from it, we no longer need this)
    SDL_FreeSurface(textSurface);
    return 0;
    //END OF NORMAL CONTROL FLOW

    //ERROR HANDLING
    error_creatingFont:
        printf("FUNC: TS_setText. TTF_Error: %s.", TTF_GetError());
        return -1;
    error_creatingTexture:
        printf("FUNC: TS_setText. SDL_Error: %s.", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return -1;
    error_creatingSurface:
        printf("FUNC: TS_setText. TTF_Error: %s.", TTF_GetError());
        return -1;
}

/**
 * @brief Sets the position of a text object.
 */
void TS_setPos(int x, int y, TextSprite *ptr) {
    ptr->dest.x = x;
    ptr->dest.y = SCREEN_HEIGHT - y - ptr->dest.h;
}

/**
 * @brief Render the text held by te text object at the previously set position.
 */
void TS_render(TextSprite *ptr) {
    SDL_RenderCopy(gRenderer, ptr->textTure, NULL, &ptr->dest);
}

/**
 * @brief Get the width of the SDL_Texture held by this text object.
 */
int TS_getWidth(TextSprite *ptr) {
    return ptr->dest.w;
}

/**
 * @brief Get the height of the SDL_Texture held by this text object.
 */
int TS_getHeight(TextSprite *ptr) {
    return ptr->dest.h;
}
