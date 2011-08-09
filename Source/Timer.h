#ifndef TIMER_H
#define TIMER_H

#include <SDL.h>

class Timer
{
public:
    Timer();
    ~Timer();

    void start();
    int getTicks();
    void setFrameRate(const unsigned int newFrameRate){this->FRAME_RATE = newFrameRate;}

    unsigned int FRAME_RATE;

private:
    int ticks;
};

#endif // TIMER_H