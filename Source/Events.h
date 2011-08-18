#ifndef EVENTS_H
#define EVENTS_H

#include <SDL.h>

#define SPEED 10

class Events
{
public:
    Events(){}
    ~Events(){}

    void handleMenuEvents(bool* quit, int* mouseX, int* mouseY,
        int* mouseClickX, int* mouseClickY, bool* clicked);

    void handleGameEvents(bool* quit, int* y_pos, bool* fullscreen);

    void handleQuit(bool* quit);

private:
    SDL_Event   event;
};

#endif // EVENTS_H