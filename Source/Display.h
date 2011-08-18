/*****************************************************
*                                                    *
*                    Display.h                       *
*                                                    *
*   A class designed to ease setting up SDL and      *
*   blitting to the screen, as well as loading       *
*   image files. Requires SDL and SDL_Image to be    *
*   installed. Automatically links to the necessary  *
*   libraries, assuming they are in the project's    *
*   path.                                            *
*                                                    *
*   METHODS:                                         *
*                                                    *
*       - blit(destination, source, x, y)            *
*           An easier way to use SDL_BlitSurface(),  *
*           very similar to PyGame's blit()          *
*           function.                                *
*                                                    *
*       - update()                                   *
*           The same as SDL_Flip(screen), faster.    *
*                                                    *
*       - loadImage(filename)                        *
*           Returning an SDL_Surface*, it loads any  *
*           image that is given using the SDL_Image  *
*           library. It returns an optimized image   *
*           if it works, the regular if it doesn't.  *
*                                                    *
*       - loadAlphaImage(filename)                   *
*           If the image you want to load has alpha  *
*           pixels, or transparency, this is the     *
*           method to call. It returns an            *
*           SDL_Surface* with all of the white       *
*           normally left by default, removed.       *
*                                                    *
*                                                    *
*   VARIABLES:                                       *
*                                                    *
*       - icon                                       *
*           An SDL_Surface for the icon in the top-  *
*           left corner and the taskbar.             *
*                                                    *
*       - doubleBuffer                               *
*           An SDL_Surface* for use with double      *
*           buffering. You can use the special flag  *
*           on initialization instead if you wish,   *
*           but I don't know how good that one is.   *
*           To use a double buffer, blit everything  *
*           on this surface, then blit this surface  *
*           on the screen, then update(). It will    *
*           smooth animations and enhance graphics.  *
*                                                    *
*       - screen                                     *
*           The main display SDL_Surface* returned   * 
*           on SDL_SetVideoMode().                   *
*                                                    *
*       - background                                 *
*           The bottom layer on the screen.          *
*                                                    *
*                                                    *
*****************************************************/

#ifndef DISPLAY_H
#define DISPLAY_H

// For filenames
#ifdef __cplusplus
#include <string>
#endif

// Make sure you have these installed
#include <SDL.h>
#include <SDL_image.h>

// To handle errors
#include "Errors.h"

// For basic SDL stuff
#include "SDL_Functions.h"

// If you are using Windows with Microsoft VC++, this
// will link the libraries for you, provided they are
// in the compiler's search path.

#ifdef _WIN32
#pragma comment(lib, "SDL_image.lib")
#endif

// Change these to fit your desires
#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600
#define SCREEN_BPP      32
// This should NOT have SDL_FULLSCREEN in it
// Use Display::toggleFullscreen() instead
#define SCREEN_FLAGS    SDL_SWSURFACE

// Libraries to initialize
#define INIT_PNG_LIB    true

// The text in the menu bar at the top
#define CAPTION         "NPong"

// Change these to customize what gets initialized in SDL
#define INITIALIZATION_FLAGS SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO

// For ease in blitting to the screen, but
// you must have a Display* class named
// "display".
#define BLIT(surf, x, y) display->blit(display->screen, surf, x, y)

// For filenames to be cross-platform
#ifdef _WIN32
#define FN_SLASH        "\\"
#else
#define FN_SLASH        "/"
#endif // _WIN32

// The location of your applications icon
#define ICON            "Data"FN_SLASH"NPong.png"

class Display
{
public:
    Display();
    ~Display();

    void toggleFullscreen();
    void blit(SDL_Surface* destination, SDL_Surface* source, const unsigned int x, const unsigned int y);
    void update();

#ifdef __cplusplus
    SDL_Surface* loadImage(const std::string& filename);
    SDL_Surface* loadAlphaImage(const std::string& filename);
#endif // __cplusplus
    SDL_Surface* loadImage(const char* filename);
    SDL_Surface* loadAlphaImage(const char* filename);

    SDL_Surface*    icon;
    SDL_Surface*    screen;
    SDL_Surface*    background;

    bool            fullscreen;
};

#endif // DISPLAY_H

