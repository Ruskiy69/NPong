#include "SDL_Functions.h"

using namespace std;

SDL_Rect createRect(const int x, const int y, const int w, const int h)
{
    SDL_Rect rect = {x, y, w, h};
    return rect;
}

SDL_Color createColor(const int r, const int g, const int b)
{
    SDL_Color color = {r, g, b, 0};
    return color;
}

Uint32 createUint32Color(SDL_Surface* source, SDL_Color color)
{
    return SDL_MapRGB(source->format, color.r, color.g, color.b);
}

SDL_Surface* createSurface(const int w, const int h, const SDL_Color color, int flags)
{
    SDL_Surface* final = SDL_CreateRGBSurface(flags, w, h, DEFAULT_DEPTH, rmask, gmask, bmask, amask);
    fillSurface(final, color);
    return final;
}

SDL_Surface* createSurface(const int w, const int h)
{
    return SDL_CreateRGBSurface(DEFAULT_FLAGS, w, h, DEFAULT_DEPTH, rmask, gmask, bmask, amask);
}

SDL_Surface* createAlphaSurface(const int w, const int h)
{
    return SDL_CreateRGBSurface(DEFAULT_FLAGS | SDL_SRCALPHA | SDL_RLEACCEL,
                w, h, DEFAULT_DEPTH, rmask, gmask, bmask, amask);
}

SDL_Surface* createAlphaSurface(const int w, const int h, SDL_Color color)
{
    SDL_Surface* final = createSurface(w, h, color, DEFAULT_FLAGS 
        | SDL_SRCALPHA | SDL_RLEACCEL);
    fillSurface(final, color);
    return final;
}

void removeColor(SDL_Surface* source, SDL_Color color)
{
    Uint32 colorkey  = createUint32Color(source, color);
    SDL_SetColorKey(source, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
}

void fillSurface(SDL_Surface* source, SDL_Color color)
{
    SDL_FillRect(source, NULL, SDL_MapRGB(source->format, color.r, color.g, color.b));
}

void fillRect(SDL_Surface* source, SDL_Rect* box, SDL_Color color)
{
    SDL_FillRect(source, box, createUint32Color(source, color));
}

SDL_Surface* renderMultiLineText(TTF_Font* font, std::string text, SDL_Color bgColor, SDL_Color fgColor, int flags)
{
    if(font == NULL)
        return NULL;

    int skipSize    = TTF_FontLineSkip(font);
    int height      = TTF_FontHeight(font);
    int width       = 0;
    int index       = -1;

    SDL_Surface* textSurface = NULL;
    vector<string> lines;

    do
    {
        int w = 0;
        index = text.find("\n", 0);

        lines.push_back(text.substr(0, index));

        if(index != -1)
        {
            text = text.substr(index + 1, -1);
        }
        TTF_SizeText(font, lines.at(lines.size() - 1).c_str(), &w, &height);
        if(width < w)
        {
            width = w;
        }
    }
    while(index != -1);

    height = (lines.size() - 1) * skipSize + height;

    if(flags & TRANSPARENT_BG)
        textSurface = createAlphaSurface(width, height);
    else
        textSurface = createSurface(width, height, bgColor);

    SDL_Surface* lineText = NULL;

    for(unsigned int i = 0; i < lines.size(); i++)
    {
        if(lines[i] == "")
            lineText = renderText(font, " ", fgColor);
        else
            lineText = renderText(font, lines.at(i), fgColor);

        if(flags & TRANSPARENT_BG)
            SDL_SetAlpha(lineText, 0, SDL_ALPHA_TRANSPARENT);

        if(flags & ALIGN_CENTER)
        {
            int w = 0;
            int h = 0;
            TTF_SizeText(font, lines.at(i).c_str(), &w, &h);
            SDL_BlitSurface(lineText, NULL, textSurface, &(SDL_Rect)createRect(width / 2 - w / 2, i * skipSize, 0, 0));
        }
        else
        {
            SDL_BlitSurface(lineText, NULL, textSurface, &(SDL_Rect)createRect(0, i * skipSize, 0, 0));
        }
        SDL_FreeSurface(lineText);
    }

    if(flags & CREATE_SURFACE)
        return textSurface;
    else
        return NULL;
}

SDL_Surface* renderText(TTF_Font* font, string text, SDL_Color color)
{
    return TTF_RenderText_Blended(font, text.c_str(), color);
}

SDL_Surface* tileSurface(SDL_Surface* source, const int screenWidth, const int screenHeight)
{
    SDL_Surface* tiledSurf    = createSurface(screenWidth, screenHeight, createColor(BLACK));
    const int surfWidth        = source->clip_rect.w;
    const int surfHeight    = source->clip_rect.h;

    int surfsMade            = 0;
    int widthTaken            = 0;
    int heightTaken            = 0;

    int current_x            = 0;
    int current_y            = 0;

    SDL_Rect whereToBlit    = createRect(current_x, current_y, 0, 0);

    while(true)
    {
        SDL_BlitSurface(source, NULL, tiledSurf, &whereToBlit);

        if(widthTaken >= screenWidth && !(heightTaken >= screenHeight))
            // Move to the next line
        {
            widthTaken     = 0;
            heightTaken    += surfHeight;
            current_y    += surfHeight;
            current_x    = 0;
        }
        else
        {
            // Move one tile over
            current_x   += surfWidth;
        }

        if(widthTaken >= screenWidth && heightTaken >= screenHeight)
            // Meaning the whole screen is tiled
            break;

        widthTaken    += surfWidth;

        whereToBlit.x = current_x;
        whereToBlit.y = current_y;

        surfsMade++;
    }

    return tiledSurf;
}

SDL_Surface* tileSurface(SDL_Surface* source, const int start_x, const int start_y, const int end_x, const int end_y)
{
    SDL_Surface* tiledSurf    = createSurface(end_x - start_x, end_y - start_y, createColor(BLACK));

    int x = 0;
    int y = 0;

    int max_w = end_x - start_x;
    int max_h = end_y - start_y;

    const int w = source->clip_rect.w;
    const int h = source->clip_rect.h;

    int w_taken = 0;
    int h_taken = 0;

    SDL_Rect blitRect = createRect(x, y, 0, 0);

    while(true)
    {
        SDL_BlitSurface(source, NULL, tiledSurf, &blitRect);

        if(w_taken >= max_w && h_taken >= max_h)
            break;

        if(x >= end_x && !(y >= end_y))
        {
            x = 0;
            w_taken = 0;
            y += h;
            h_taken += h;
        }
        else
        {
            x += w;
            w_taken += w;
        }

        blitRect.x = x;
        blitRect.y = y;
    }
    return tiledSurf;
}

void fadeToColor(SDL_Surface* screen, SDL_Color color)
{
    float alpha                = SDL_ALPHA_OPAQUE;
    float alpha_incr        = 10;

    SDL_Surface* black        = createAlphaSurface(DEFAULT_WIDTH, DEFAULT_HEIGHT, createColor(BLACK));
    SDL_Surface* colorSurf    = createAlphaSurface(DEFAULT_WIDTH, DEFAULT_HEIGHT, color);

    while(alpha > 0)
    {
        SDL_SetAlpha(colorSurf, SDL_SRCALPHA, (Uint8)(255 - alpha));
        SDL_BlitSurface(black, NULL, screen, NULL);
        SDL_BlitSurface(colorSurf, NULL, screen, NULL);
        SDL_Flip(screen);
        SDL_Delay(20);
        alpha -= alpha_incr;
    }
}

void fadeSurfThruSurf(SDL_Surface* screen, SDL_Surface* bottomSurface, SDL_Surface* topSurface)
{
    for(int i = SDL_ALPHA_TRANSPARENT;
        i < SDL_ALPHA_OPAQUE; 
        i = i + 3)
    {
        SDL_BlitSurface(bottomSurface, NULL, screen, NULL);
        SDL_BlitSurface(topSurface, NULL, screen, NULL);
        SDL_SetAlpha(bottomSurface, SDL_SRCALPHA, i);
        SDL_BlitSurface(bottomSurface, NULL, screen, NULL);
        SDL_SetAlpha(bottomSurface, 0, SDL_ALPHA_OPAQUE);
        SDL_Flip(screen);
    }
}

int getTextWidth(TTF_Font* font, std::string text)
{
    int width = 0;
    TTF_SizeText(font, text.c_str(), &width, NULL);
    return width;
}

int getTextHeight(TTF_Font* font, std::string text)
{
    int height = 0;
    TTF_SizeText(font, text.c_str(), NULL, &height);
    return height;
}
bool detectCollision(const SDL_Rect& obj1, const SDL_Rect& obj2)
{
    int top1, bottom1, right1, left1;
    int top2, bottom2, right2, left2;

    left1   = obj1.x;
    right1  = obj1.x + obj1.w;
    top1    = obj1.y;
    bottom1 = obj1.y + obj1.h;

    left2   = obj2.x;
    right2  = obj2.x + obj2.w;
    top2    = obj2.y;
    bottom2 = obj2.y + obj2.h;

    if(bottom1 <= top2)
        return false;
    if(top1 >= bottom2)
        return false;
    if(right1 <= left2)
        return false;
    if(left1 >= right2)
        return false;

    return true;
}