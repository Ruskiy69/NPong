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
    Uint32 oldTicks = this->ticks;
    this->ticks = SDL_GetTicks();
    return this->ticks - oldTicks;
}
