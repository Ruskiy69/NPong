#include "Timer.h"


Timer::Timer()
{
    this->FRAME_RATE = 45;
    this->ticks = 0;
}

Timer::~Timer()
{
}

void Timer::start()
{
    this->ticks = SDL_GetTicks();
}

int Timer::getTicks()
{
    return SDL_GetTicks() - this->ticks;
}
