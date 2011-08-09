#include "MenuCreation.h"

Menu::Menu()
{
    if(!this->initLibs())
        handleError(SDL_GetError());

    this->display       = new Display;
    this->eventHandler  = new Events;
    this->quit          = false;
    this->music         = NULL;
    this->music_chunk   = NULL;
}

Menu::Menu(Display* display)
{
    if(!this->initLibs())
        handleError(SDL_GetError());

    this->display       = display;
    this->eventHandler  = new Events;
    this->quit          = false;
    this->music         = NULL;
    this->music_chunk   = NULL;
}

Menu::Menu(Display* display, Events* eventHandler)
{
    if(!this->initLibs())
        handleError(SDL_GetError());

    this->display       = display;
    this->eventHandler  = eventHandler;
    this->quit          = false;
    this->music         = NULL;
    this->music_chunk   = NULL;
}

bool Menu::initLibs()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == -1)
        return false;
    if(TTF_Init() == -1)
        return false;
    if(Mix_Init(MIX_INIT_MP3) == 0)
        return false;
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
        return false;
    return true;
}

Menu::~Menu()
{
    for(unsigned int i=0; i < this->buttons.size(); i++)
    {
        delete this->buttons[i];
    }
    
    this->buttons.clear();

    if(this->music != NULL)
        Mix_FreeMusic(this->music);
    if(this->music_chunk != NULL)
        Mix_FreeChunk(this->music_chunk);

     /* You should call IMG_Quit, TTF_Quit,
      * Mix_Quit, Mix_CloseAudio, and 
      * SDL_Quit on your own.
      */       
}

void Menu::setBackground(SDL_Surface* bg)
{
    this->bg = bg;
}

void Menu::setBackground(const char* filename)
{
    this->bg = this->display->loadAlphaImage(filename);
}

void Menu::setBackground(const string& filename)
{
    this->setBackground(filename.c_str());
}

void Menu::setFont(TTF_Font* font)
{
    this->font = font;
}

void Menu::setFont(const char* font_name)
{
    this->font = TTF_OpenFont(font_name, 48);
    this->inbetween = TTF_FontHeight(this->font);
    if(!this->font)
        handleError(TTF_GetError());
}

void Menu::setFont(const string& font_name)
{
    this->setFont(font_name.c_str());
}

void Menu::setTextColor(const int r, const int g, const int b)
{
    this->fgColor = createColor(r, g, b);
}

void Menu::setTextColor(const SDL_Color& color)
{
    this->fgColor = color;
}

void Menu::setHighLightColor(const int r, const int g, const int b)
{
    this->hlColor = createColor(r, g, b);
}

void Menu::setHighLightColor(const SDL_Color& color)
{
    this->hlColor = color;
}

void Menu::setMusic(const char* file)
{
    this->music = Mix_LoadMUS(file);
}

void Menu::setMusic(const string& file)
{
    this->setMusic(file.c_str());
}

void Menu::setMusic(Mix_Chunk* music)
{
    this->music = NULL;
    this->music_chunk = music;
}

void Menu::setMusic(Mix_Music* music)
{
    this->music = music;
    this->music_chunk = NULL;
}

void Menu::setStartCoordinates(const int x, const int y)
{
    this->x = x;
    this->y = y;
}

void Menu::setStartCoordinatesInScreenCenter()
{
    this->setCenterText(true);
    this->y             = SCREEN_WIDTH / 2;
}

void Menu::setCenterText(bool value)
{
    this->centerText = value;
}

void Menu::newText(const char* text)
{
    this->buttons.push_back(new Button);
    this->buttons[LAST]->type = TEXT;
    this->buttons[LAST]->button_text = new char[strlen(text) + 1];
    strncpy(this->buttons[LAST]->button_text, text, strlen(text) + 1);

    if(this->centerText)
    {
        this->buttons[LAST]->normalButton = renderMultiLineText(
            this->font, text, this->hlColor, this->fgColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->x = ((SCREEN_WIDTH / 2) - 
            (this->buttons[LAST]->normalButton->clip_rect.w / 2));

        this->inbetween = this->buttons[LAST]->normalButton->clip_rect.h;
        this->buttons[LAST]->y = this->y;
        this->y += this->inbetween;
    }
    else
    {
        this->buttons[LAST]->normalButton = renderMultiLineText(
            this->font, (string)text, this->hlColor, this->fgColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->inbetween = this->buttons[LAST]->normalButton->clip_rect.h;
        this->buttons[LAST]->x = this->x;
        this->buttons[LAST]->y = this->y;
        this->y += this->inbetween;
    }

    this->buttons[LAST]->mouseOverBox = createRect(this->buttons[LAST]->x,
        this->buttons[LAST]->y,
        this->buttons[LAST]->normalButton->clip_rect.w,
        this->buttons[LAST]->normalButton->clip_rect.h);

    this->buttons[LAST]->action = NO_ACTION;
    this->buttons[LAST]->toggle = NO_TOGGLE;
    this->buttons[LAST]->highlights = false;
}

void Menu::newSubMenu(const char* text, Menu* nextMenu)
{
    this->buttons.push_back(new Button);

    if(this->centerText)
    {
        this->buttons[LAST]->normalButton = renderMultiLineText(
            this->font, text, this->hlColor, this->fgColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlightedButton = renderMultiLineText(
            this->font, text, this->fgColor, this->hlColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->x = ((SCREEN_WIDTH / 2) - 
            (this->buttons[LAST]->normalButton->clip_rect.w / 2));

        this->inbetween = this->buttons[LAST]->normalButton->clip_rect.h;
        this->buttons[LAST]->y = this->y;
        this->y += this->inbetween;
    }
    else
    {
        this->buttons[LAST]->normalButton = renderMultiLineText(
            this->font, (string)text, this->hlColor, this->fgColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlightedButton = renderMultiLineText(
            this->font, (string)text, this->fgColor, this->hlColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->inbetween = this->buttons[LAST]->normalButton->clip_rect.h;
        this->buttons[LAST]->x = this->x;
        this->buttons[LAST]->y = this->y;
        this->y += this->inbetween;
    }

    this->buttons[LAST]->mouseOverBox = createRect(this->buttons[LAST]->x,
        this->buttons[LAST]->y,
        this->buttons[LAST]->normalButton->clip_rect.w,
        this->buttons[LAST]->normalButton->clip_rect.h);

    this->buttons[LAST]->type = SUB_MENU;
    this->buttons[LAST]->nextMenu = nextMenu;
    this->buttons[LAST]->action = NO_ACTION;
    this->buttons[LAST]->toggle = NO_TOGGLE;
    this->buttons[LAST]->highlights = true;
}

void Menu::newToggle(const char* text, TOGGLE toggle_type, bool on)
{
    this->buttons.push_back(new Button);

    if(this->centerText)
    {
        this->buttons[LAST]->normalButton = renderMultiLineText(
            this->font, text, this->hlColor, this->fgColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlightedButton = renderMultiLineText(
            this->font, text, this->fgColor, this->hlColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->x = ((SCREEN_WIDTH / 2) - 
            (this->buttons[LAST]->normalButton->clip_rect.w / 2));

        this->inbetween = this->buttons[LAST]->normalButton->clip_rect.h;
        this->buttons[LAST]->y = this->y;
        this->y += this->inbetween;
    }
    else
    {
        this->buttons[LAST]->normalButton = renderMultiLineText(
            this->font, (string)text, this->hlColor, this->fgColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlightedButton = renderMultiLineText(
            this->font, (string)text, this->fgColor, this->hlColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->inbetween = this->buttons[LAST]->normalButton->clip_rect.h;
        this->buttons[LAST]->x = this->x;
        this->buttons[LAST]->y = this->y;
        this->y += this->inbetween;
    }

    this->buttons[LAST]->mouseOverBox = createRect(this->buttons[LAST]->x,
        this->buttons[LAST]->y,
        this->buttons[LAST]->normalButton->clip_rect.w,
        this->buttons[LAST]->normalButton->clip_rect.h);

    this->buttons[LAST]->type = TOGGLE_MENU;
    this->buttons[LAST]->button_text = new char[strlen(text) + 1];
    strncpy(this->buttons[LAST]->button_text, text, strlen(text) + 1);
    this->buttons[LAST]->action = NO_ACTION;
    this->buttons[LAST]->toggle = toggle_type;
    this->buttons[LAST]->highlights = true;
    this->buttons[LAST]->switch_status = on;
}

void Menu::newActionOption(const char* text, ACTION action_type)
{
    this->buttons.push_back(new Button);

    if(this->centerText)
    {
        this->buttons[LAST]->normalButton = renderMultiLineText(
            this->font, text, this->hlColor, this->fgColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlightedButton = renderMultiLineText(
            this->font, text, this->fgColor, this->hlColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->x = ((SCREEN_WIDTH / 2) - 
            (this->buttons[LAST]->normalButton->clip_rect.w / 2));

        this->inbetween = this->buttons[LAST]->normalButton->clip_rect.h;
        this->buttons[LAST]->y = this->y;
        this->y += this->inbetween;
    }
    else
    {
        this->buttons[LAST]->normalButton = renderMultiLineText(
            this->font, (string)text, this->hlColor, this->fgColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlightedButton = renderMultiLineText(
            this->font, (string)text, this->fgColor, this->hlColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->inbetween = this->buttons[LAST]->normalButton->clip_rect.h;
        this->buttons[LAST]->x = this->x;
        this->buttons[LAST]->y = this->y;
        this->y += this->inbetween;
    }

    this->buttons[LAST]->mouseOverBox = createRect(this->buttons[LAST]->x,
        this->buttons[LAST]->y,
        this->buttons[LAST]->normalButton->clip_rect.w,
        this->buttons[LAST]->normalButton->clip_rect.h);

    this->buttons[LAST]->type = ACTION_MENU;
    this->buttons[LAST]->action = action_type;
    this->buttons[LAST]->toggle = NO_TOGGLE;
    this->buttons[LAST]->highlights = true;
}

void Menu::checkMouseOver(int mouseX, int mouseY)
{
    for(unsigned int i=0; i < this->buttons.size(); i++)
    {
        if(detectCollision(this->buttons[i]->mouseOverBox, createRect(mouseX, mouseY, 0, 0)) &&
            (this->buttons[i]->highlightedButton != NULL) && this->buttons[i]->highlights)
        {
            this->buttons[i]->displayButton = this->buttons[i]->highlightedButton;
        }
        else
        {
            this->buttons[i]->displayButton = this->buttons[i]->normalButton;
        }
    }
}

int Menu::checkClick(const int x, const int y)
{
    for(unsigned int i=0; i < this->buttons.size(); i++)
    {
        if(detectCollision(this->buttons[i]->mouseOverBox, createRect(x, y, 0, 0)))
            return i;
    }
    return -1;
}

int Menu::run()
{
    int mouseX, mouseY;
    int mouseClickX, mouseClickY;
    bool clicked;
    int status;

    if(this->music != NULL)
    {
        if(Mix_PlayMusic(this->music, -1) == -1)
        {
            handleError("In-game music disabled! Click OK for full error", false);
            handleError(Mix_GetError(), false);
        }
    }
    else if(this->music_chunk != NULL)
    {
        int channel = Mix_PlayChannel(-1, this->music_chunk, -1);
    }

    while(!this->quit)
    {
        this->eventHandler->handleMenuEvents(&this->quit, &mouseX, &mouseY,
            &mouseClickX, &mouseClickY, &clicked);

        this->checkMouseOver(mouseX, mouseY);

        /* Check where the user clicked */
        if(clicked)
            status = this->checkClick(mouseClickX, mouseClickY);
        else
            status = -1;

        if(status != -1)
        {
            if(this->buttons[status]->type == SUB_MENU)
            {
                if(this->buttons[status]->nextMenu->run() == -1)
                    return -1;
            }

            else if(this->buttons[status]->type == ACTION_MENU)
            {
                if(this->buttons[status]->action == QUIT_GAME)
                    return -1;
                else if(this->buttons[status]->action == RETURN_TO_LAST)
                    return 1;
                else if(this->buttons[status]->action == PLAY_GAME)
                {
                    Mix_HaltMusic();
                    return 0;
                }
                else if(this->buttons[status]->action == PLAY_MULTI_GAME)
                {
                    Mix_HaltMusic();
                    return 2;
                }
            }

            else if(this->buttons[status]->type == TOGGLE_MENU)
            {
                string tmp(this->buttons[status]->button_text);

                if(this->buttons[status]->switch_status)
                {
                    this->buttons[status]->switch_status = false;
                    tmp = tmp.substr(0, tmp.find("ON"));
                    tmp += "OFF";
                }
                else
                {
                    this->buttons[status]->switch_status = true;
                    tmp = tmp.substr(0, tmp.find("OFF"));
                    tmp += "ON";
                }

                delete[] this->buttons[status]->button_text;
                this->buttons[status]->button_text = new char[tmp.length() + 1];
                strncpy(this->buttons[status]->button_text, tmp.c_str(), tmp.length() + 1);

                if(this->centerText)
                {
                    this->buttons[status]->normalButton = renderMultiLineText(
                        this->font, tmp, this->hlColor, this->fgColor,
                        ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

                    this->buttons[status]->highlightedButton = renderMultiLineText(
                        this->font, tmp, this->fgColor, this->hlColor,
                        ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

                    this->buttons[status]->x = ((SCREEN_WIDTH / 2) - 
                        (this->buttons[status]->normalButton->clip_rect.w / 2));
                }

                else
                {
                    this->buttons[status]->normalButton = renderMultiLineText(
                        this->font, tmp, this->hlColor, this->fgColor,
                        TRANSPARENT_BG | CREATE_SURFACE);

                    this->buttons[status]->highlightedButton = renderMultiLineText(
                        this->font, tmp, this->fgColor, this->hlColor,
                        TRANSPARENT_BG | CREATE_SURFACE);
                }
            }
        }

        BLIT(this->bg, 0, 0);

        /* Display each menu option */
        for(unsigned int i=0; i < this->buttons.size(); i++)
        {
            BLIT(this->buttons[i]->displayButton,
                this->buttons[i]->x, 
                this->buttons[i]->y);
        }

        /* Update the screen */
        this->display->update();

        /* Lower CPU usage */
        SDL_Delay(100);
    }

    /* This means that the user actually wants to
     * quit, not that we just broke out of the while()
     * loop with a break statement.
    */
    if(this->quit == true)
        return -1;
    else
        return 0;
}