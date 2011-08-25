#include "Display.h"

#ifdef __cplusplus
using namespace std;
#endif

Display::Display()
{
#ifdef __cplusplus
    const string init_error = 
            "SDL failed to initialize! \n\
            You are probably missing dependencies! \n\
            Check for SDL DLL's and try again!\n\n\n\
            A full list of dependencies is in README.TXT.";

#else
    const char init_error[] = "SDL failed to initialize! \n"
                              "You are probably missing dependencies! \n"
                              "Check for SDL DLL's and try again!\n\n\n"
                              "A full list of dependencies is in README.TXT";
#endif // __cplusplus

    if(SDL_Init(INITIALIZATION_FLAGS) == -1)
    {
#ifdef __cplusplus
        handleError(init_error.c_str());
#else
        handleError(init_error);
#endif // __cplusplus
    }

    if(INIT_PNG_LIB)
    {
        if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
        {
            handleError("Unable to load .PNG library!");
        }
    }

    SDL_WM_SetCaption(CAPTION, NULL);
    //SDL_ShowCursor(SDL_DISABLE);

    if(ICON != NULL)
    {
        if(INIT_PNG_LIB)
        {
            this->icon = IMG_Load(ICON);
            if(!this->icon)
            {
                handleError(SDL_GetError());
            }

            SDL_WM_SetIcon(this->icon, NULL);
            SDL_FreeSurface(this->icon);
        }
        else
        {
            this->icon       = SDL_LoadBMP(ICON);
            if(!this->icon)
            {
                handleError(SDL_GetError());
            }

            SDL_WM_SetIcon(this->icon, NULL);

            SDL_FreeSurface(this->icon);
        }
    }

    this->fullscreen    = false;
    this->screen        = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SCREEN_FLAGS);

    if(!this->screen)
    {
        handleError(SDL_GetError());
    }

    this->background = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, WHITE, 0);
    SDL_FillRect(this->background, NULL, 0x000000);
}

Display::~Display()
{
    SDL_FreeSurface(this->background);

    if(INIT_PNG_LIB)
        IMG_Quit();
    SDL_Quit();
}

void Display::toggleFullscreen()
{
    this->fullscreen = !this->fullscreen;

    Uint32 flags = SCREEN_FLAGS;

    if(this->fullscreen)
        flags |= SDL_FULLSCREEN;

    SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, flags);
}

void Display::blit(SDL_Surface* destination, SDL_Surface* source, const unsigned int x, const unsigned int y)
{
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(source, NULL, destination, &offset);
}

void Display::update()
{
    SDL_Flip(this->screen);
}

#ifdef __cplusplus
SDL_Surface* Display::loadImage(const string& filename)
{
    return this->loadImage(filename.c_str());
}

SDL_Surface* Display::loadAlphaImage(const string& filename)
{
    return this->loadAlphaImage(filename.c_str());
}
#endif // __cplusplus

SDL_Surface* Display::loadImage(const char* filename)
{
    SDL_Surface* image     = NULL;
    SDL_Surface* optimized = NULL;

    if(INIT_PNG_LIB)
    {
        image = IMG_Load(filename);
    }
    else
    {
        image = SDL_LoadBMP(filename);
    }

    if(!image)
    {
        handleError(SDL_GetError());
    }

    optimized = SDL_DisplayFormat(image);

    if(!optimized)
    {
        SDL_FreeSurface(optimized);
        return image;
    }
    else
    {
        SDL_FreeSurface(image);
        return optimized;
    }
}

SDL_Surface* Display::loadAlphaImage(const char* filename)
{
    SDL_Surface* image     = NULL;
    SDL_Surface* optimized = NULL;

    if(INIT_PNG_LIB)
    {
        image = IMG_Load(filename);
    }
    else
    {
        image = SDL_LoadBMP(filename);
    }

    if(!image)
    {
        handleError(SDL_GetError());
    }

    optimized = SDL_DisplayFormatAlpha(image);

    if(!optimized)
    {
        SDL_FreeSurface(optimized);
        Uint32 colorkey        = SDL_MapRGB(image->format, WHITE);
        SDL_SetColorKey(image, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        return image;
    }
    else
    {
        SDL_FreeSurface(image);
        Uint32 colorkey        = SDL_MapRGB(optimized->format, WHITE);
        SDL_SetColorKey(optimized, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        return optimized;
    }
}