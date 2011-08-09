#include "Events.h"

using namespace std;

void Events::handleMenuEvents(bool* quit, int* mouseX, int* mouseY,
        int* mouseClickX, int* mouseClickY, bool* clicked)
{
    while(SDL_PollEvent(&this->event))
    {
        switch(this->event.type)
        {
        case SDL_QUIT:
            *quit = true;
            return;
        case SDL_KEYDOWN:
            if(this->event.key.keysym.sym == SDLK_ESCAPE ||
                this->event.key.keysym.sym == SDLK_q)
            {
                *quit = true;
                return;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(this->event.button.button == SDL_BUTTON_LEFT)
            {
                *clicked = true;
                *mouseClickX = this->event.button.x;
                *mouseClickY = this->event.button.y;
                SDL_GetMouseState(mouseX, mouseY);
                return;
            }
            else
            {
                break;
            }
        default:
            SDL_GetMouseState(mouseX, mouseY);
            break;
        }
    }
    *clicked = false;
}

void Events::handleGameEvents(bool* quit, int* dy)
{
	while(SDL_PollEvent(&this->event))
	{
		switch(this->event.type)
		{
		case SDL_QUIT:
			*quit = true;
			break;
		case SDL_KEYDOWN:
			switch(this->event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
				*quit = true;
				break;
			case SDLK_UP:
			case SDLK_w:
				*dy = -SPEED;
				break;
			case SDLK_DOWN:
			case SDLK_s:
				*dy =  SPEED;
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch(this->event.key.keysym.sym)
			{
			case SDLK_UP:
			case SDLK_DOWN:
			case SDLK_w:
			case SDLK_s:
				*dy = 0;
				break;
			default: 
				break;
			}
			break;
		default:
			break;
		}
	}
}

void Events::handleQuit(bool* quit)
{
    while(SDL_PollEvent(&this->event))
    {
        if(event.type == SDL_QUIT)
            *quit = true;
        if(event.type == SDL_KEYDOWN)
            if(this->event.key.keysym.sym == SDLK_ESCAPE ||
                this->event.key.keysym.sym == SDLK_q)
            {
                *quit = true;
                return;
            }
            break;
    }
}