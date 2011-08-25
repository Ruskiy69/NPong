#ifndef ENGINE_H
#define ENGINE_H

/* C++ strings and stringstreams
 * for easier logging.
 */
#include <string>
#include <sstream>

/* Wrapper classes for basically everything */
#include "Errors.h"
#include "SDL_Functions.h"
#include "Display.h"
#include "Events.h"
#include "Entity.h"
#include "MenuCreation.h"
#include "Timer.h"

/* Multiplayer gaming, and error logging */
#include "Socket.h"
#include "Logging.h"

/* Game constants */
static const int FPS            = 60;

static const int PONG_HEIGHT    = 40;
static const int PONG_WIDTH     = 10;

static const int P1X_START      = 0;
static const int P2X_START      = SCREEN_WIDTH - PONG_WIDTH;
static const int P1Y_START      = SCREEN_HEIGHT / 2;
static const int P2Y_START      = SCREEN_HEIGHT / 2;
static const int BALL_X_START   = SCREEN_WIDTH / 2;
static const int BALL_Y_START   = SCREEN_HEIGHT / 2;

/* Split a string everywhere it finds a 
 * certain character. Returns a vector.
 */
vector<string> split(char* str, char token);

/* One logging stringstream to rule them all */
static stringstream logger;

class Engine
{
public:
    Engine();
    ~Engine();

    void menu();

private:
    void playGame();
    void setupMulti(bool scanNetwork = false);
    void playMultiGame();
    void sendPosition();
    void recvPosition(int* y);
    void delayFps();

    /* For singleplayer ai */
    void calcMove(int* ai_dy, const int dx, const int dy);

    /* For logging */
    Log*        log;

    /* Game functionality */
    Display*    display;
    Events*     eventHandler;
    Timer*      fps;

    /* The game menus */
    Menu*       mainMenu;
    Menu*       mpMenu;

    /* The objects */
    Entity*     p1;
    Entity*     p2;
    Entity*     ball;

    /* For multiplayer */
    Socket*     peer;
    int         isHost;

    /* A black background */
    SDL_Surface*    bg;

    /* Score handling */
    TTF_Font*   score_font;
    int         p1_score;
    int         p2_score;
};

#endif // ENGINE_H