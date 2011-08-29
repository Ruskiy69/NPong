#ifndef SDL_FUNC_H
#define SDL_FUNC_H

#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define rmask            0xff000000
#define gmask            0x00ff0000
#define bmask            0x0000ff00
#define amask            0x000000ff
#else
#define rmask            0x000000ff
#define gmask            0x0000ff00
#define bmask            0x00ff0000
#define amask            0xff000000
#endif

#define DEFAULT_WIDTH   800
#define DEFAULT_HEIGHT  600
#define DEFAULT_DEPTH   32
#define DEFAULT_FLAGS   SDL_SWSURFACE | SDL_SRCCOLORKEY

#define ALIGN_CENTER    0x01
#define CREATE_SURFACE  0x02
#define TRANSPARENT_BG  0x04

#define WHITE           0xFF, 0xFF, 0xFF
#define BLACK           0x00, 0x00, 0x00
#define RED             0xFF, 0x00, 0x00
#define GREEN           0x00, 0xFF, 0x00
#define BLUE            0x00, 0x00, 0xFF
#define YELLOW          0xFF, 0x00, 0xFF
#define PURPLE          0xFF, 0x00, 0xFF

#ifdef _WIN32
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLMain.lib")
#pragma comment(lib, "SDL_ttf.lib")
#endif // _WIN32

SDL_Rect  createRect(const int x, const int y, const int w, const int h);
SDL_Color createColor(const int r, const int g, const int b);
Uint32    createUint32Color(SDL_Surface* source, SDL_Color color);

SDL_Surface* createSurface(const int w, const int h);
SDL_Surface* createAlphaSurface(const int w, const int h);
SDL_Surface* createAlphaSurface(const int w, const int h, SDL_Color color);
SDL_Surface* createSurface(const int w, const int h, const SDL_Color color, int flags = DEFAULT_FLAGS);
SDL_Surface* renderMultiLineText(TTF_Font* font, std::string text, SDL_Color bgColor, SDL_Color fgColor, int flags);
SDL_Surface* renderText(TTF_Font* font, std::string text, SDL_Color color);
SDL_Surface* tileSurface(SDL_Surface* source, const int screenWidth, const int screenHeight);
SDL_Surface* tileSurface(SDL_Surface* source, const int start_x, const int start_y, const int end_x, const int end_y);

void removeColor(SDL_Surface* source, SDL_Color color);
void fillSurface(SDL_Surface* source, SDL_Color color);
void fillRect(SDL_Surface* source, SDL_Rect* box, SDL_Color color);

void fadeToColor(SDL_Surface* screen, SDL_Color color);
void fadeSurfThruSurf(SDL_Surface* screen, SDL_Surface* bottomSurface, SDL_Surface* topSurface);

int getTextWidth(TTF_Font* font, std::string text);
int getTextHeight(TTF_Font* font, std::string text);

bool detectCollision(const SDL_Rect& obj1, const SDL_Rect& obj2);

#endif // SDL_FUNC_H