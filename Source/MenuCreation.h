/* A few simple classes to help create text-based
 * menus and sub-menus.
 *
 *
 * An example of a main menu with one submenu, hierarchy
 * is like so:
 * Main-Menu
 *  - Play Game
 *  - Options
 *    - Sound FX: ON/OFF
 *    - Music: OFF/ON
 *  - Quit
 * Code:
 *
 * #include "MenuCreation.h"
 * class Engine
 * {
 * public:
 *      Engine();
 *      ~Engine();
 *      void mainMenu();
 * private:
 *      Display* display;
 *      Menu*    mainMenu;
 *      Menu*    optionsMenu;
 * };
 *
 * void Engine::mainMenu()
 * {
 *      this->mainMenu = new Menu(this->display);
 *      this->optionsMenu = new Menu(this->display);
 *
 *      // Load a background image, font, & position
 *      this->mainMenu->setBackground("Img"FN_SLASH"BG.png");
 *      this->mainMenu->setFont("menu.ttf");
 *      this->mainMenu->setCoordinates(SCREEN_WIDTH / 2, 300);
 *      this->mainMenu->setCenter();// Align the text center
 *
 *      this->optionsMenu->setBackground(this->display->loadImage("Img"FN_SLASH"BG.png");
 *      this->optionsMenu->setFont("menu.ttf");
 *      this->optionsMenu->setCoordinatesInScreenCenter();  // Put text in middle of screen
 *      
 *      // Set options
 *      this->mainMenu->setActionOption("Play Game", PLAY_GAME);
 *      this->mainMenu->setSubMenu("Options", this->optionsMenu);
 *      this->mainMenu->setActionOption("Quit", QUIT_GAME);
 *
 *      this->optionsMenu->newText("This is some help stuff");
 *      this->optionsMenu->newToggle("Sound FX: ON", TOGGLE, true); // The "true" part sets the default value for the toggle to "ON"
 *      this->optionsMenu->newToggle("Music: OFF", TOGGLE, false);
 *      this->optionsMenu->newActionOption("Return", RETURN_TO_LAST);
 *
 * 	// Finally, run the main menu
 *      int retval = this->mainMenu->run();
 *
 *	// Check if user wanted to quit
 *	if(retval == -1)
 *	    exit(0);
 * End of example
 */

#ifndef MENU_CREATOR_H
#define MENU_CREATOR_H

/* Standard containers and strings */
#include <vector>
#include <string>

/* SDL includes */
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#ifdef WIN32
    /* Link all of the necessary libraries */
    #pragma comment(lib, "SDLmain.lib")
    #pragma comment(lib, "SDL.lib")
    #pragma comment(lib, "SDL_ttf.lib")
    #pragma comment(lib, "SDL_image.lib")
    #pragma comment(lib, "SDL_mixer.lib")
#endif // WIN32

/* SDL wrappers, error handling*/
#include "Errors.h"
#include "Display.h"
#include "Events.h"
#include "SDL_Functions.h"

/* Dirty macro to access last element of vector */
#define LAST	this->buttons.size() - 1

using namespace std;

/* Various types of toggles */
enum TOGGLE 
{
    NO_TOGGLE = -1,
    SWITCH,
    SLIDER,
};

enum ACTION
{
    NO_ACTION = -1,
    QUIT_GAME,
    RETURN_TO_LAST,
    LOAD_GAME,
    PLAY_GAME,
    PLAY_MULTI_GAME
};

enum BUTTON_TYPE
{
    SUB_MENU,
    ACTION_MENU,
    TOGGLE_MENU,
    TEXT
};

/* Forward declaration so the Button struct
 * can recognize it.
 */
class Menu;

/* A generic button that holds images
 * and position data. It can become a 
 * Sub Menu, a simple Text option, a
 * Toggle for various settings, or
 * can perform an action of some kind.
 */
struct Button
{
    /* Buttons and collision box */
    SDL_Surface*    normalButton;   // Always
    SDL_Surface*    displayButton;  // Always
    SDL_Surface*    highlightedButton;  // If sub-menu or toggle
    SDL_Rect	    mouseOverBox;

    /* Sub-menus only */
    Menu*	        nextMenu;

    /* Does it highlight? */
    bool            highlights;
    /* Position */
    int             x, y;
    /* If a toggle, on or off? */
    bool            switch_status;

    /* Type specifier */
    BUTTON_TYPE     type;
    ACTION          action;
    TOGGLE          toggle;

    /* The text in the toggle */
    char* button_text;

    /* For switch toggles only */
    void reloadToggle();
};

class Menu
{
public:
    /* If default constructor is used, an Events & Display object will
     * be created anyway. Passing one will save memory I guess.
     */

    Menu();
    Menu(Display* display);
    Menu(Display* display, Events* eventHandler);
    ~Menu();

    void setBackground(SDL_Surface* bg);
    void setBackground(const char* filename);
    void setBackground(const string& filename);

    void setFont(TTF_Font* font);
    void setFont(const char* font_name);
    void setFont(const string& font_name);

    void setTextColor(const int r, const int g, const int b);
    void setTextColor(const SDL_Color& color);

    void setHighLightColor(const int r, const int g, const int b);
    void setHighLightColor(const SDL_Color& color);

    void setMusic(const char* filename);
    void setMusic(const string& filename);
    void setMusic(Mix_Chunk* music);
    void setMusic(Mix_Music* music);

    void setStartCoordinates(const int x, const int y);
    void setStartCoordinatesInScreenCenter();
    void setCenterText(bool);

    void newActionOption(const char* text, ACTION action_type);
    void newSubMenu(const char* text, Menu* nextMenu);
    void newToggle(const char* text, TOGGLE toggle_type, bool on = true);
    void newText(const char* text);

    int  run();

    SDL_Surface* getBackground(){return this->bg;}
    char*        getFileName() const{return this->filename;}


private:
    bool initLibs();
    void checkMouseOver(const int x, const int y);
    int  checkClick(const int x, const int y);

    Display*        display;
    Events*	        eventHandler;

    TTF_Font*	    font;
    SDL_Surface*    bg;
    SDL_Color	    fgColor;
    SDL_Color       hlColor;

    Mix_Music*      music;
    Mix_Chunk*      music_chunk;

    bool	        quit;
    bool	        centerText;

    int		        x, y;
    int		        inbetween;	    // Amount of pixels between each line\

    char*           filename;       // If user wants to load a file or game

    vector<Button*> buttons;
};

#endif // MENU_CREATOR_H
