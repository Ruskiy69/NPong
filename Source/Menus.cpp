#include "Menus.h"

using std::string;

Menu::Menu()
{
    this->display       = new Display();
    this->eventHandler  = new Events();

    this->Initialize();
}

Menu::Menu(Display* display)
{
    this->display       = display;
    this->eventHandler  = new Events();

    this->Initialize();
}

Menu::Menu(Display* display, Events* eventHandler)
{
    this->display       = display;
    this->eventHandler  = eventHandler;

    this->Initialize();
}

Menu::~Menu()
{
    for(unsigned int i=0; i < this->buttons.size(); i++)
    {
        delete this->buttons[i];
        this->buttons[i] = NULL;
    }

    this->buttons.clear();
}

bool Menu::Initialize()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == -1)
        return false;
    if(TTF_Init() == -1)
        return false;
    if(Mix_Init(MIX_INIT_MP3) == 0)
        return false;
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
        return false;

    /* Set some default values for menus */
#if (defined _WIN32 || defined WIN32)
    this->font      = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 32);
#else
    this->font      = NULL;
#endif // _WIN32 || WIN32
    
    this->bg        = createSurface(SCREEN_WIDTH, SCREEN_HEIGHT, createColor(BLACK));
    this->music     = NULL;
    this->txtColor  = createColor(WHITE);
    this->txtColor  = createColor(GREEN);
    this->offset    = getTextHeight(this->font, "TEST");
    this->quit      = false;
    this->setCenterText(true);
    this->setStartCoordinates(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    return true;
}

void Menu::setBackground(SDL_Surface* bg)
{
    this->bg = bg;
}

void Menu::setBackground(const char* filename)
{
    this->bg = this->display->loadImage(filename);
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
    this->font = TTF_OpenFont(font_name, 32);
}

void Menu::setFont(const string& font_name)
{
    this->setFont(font_name.c_str());
}

void Menu::setTextColor(const int r, const int g, const int b)
{
    this->txtColor = createColor(r, g, b);
}

void Menu::setTextColor(const SDL_Color& color)
{
    this->txtColor = color;
}

void Menu::setHighLightColor(const int r, const int g, const int b)
{
    this->hlColor = createColor(r, g, b);
}

void Menu::setHighLightColor(const SDL_Color& color)
{
    this->hlColor = color;
}

void Menu::setMusic(const char* filename)
{
    this->music = Mix_LoadMUS(filename);
}

void Menu::setMusic(const string& filename)
{
    this->setMusic(filename.c_str());
}

void Menu::setMusic(Mix_Music* music)
{
    this->music = music;
}

void Menu::setStartCoordinates(const int x, const int y)
{
    this->x = x;
    this->y = y;
}

void Menu::setStartCoordinatesInScreenCenter()
{
    this->x = SCREEN_WIDTH / 2;
    this->y = SCREEN_HEIGHT / 2;
}

void Menu::setCenterText(bool toggle)
{
    this->center = toggle;
}

 int Menu::addMenuOption(
            const char* text, 
            BUTTON_TYPE btn,
            ACTION_TYPE act,
            Menu*       SubMenu,
            bool        onByDefault)
{
    switch(btn)
    {
        case BTN_GENERIC:
            return this->addGeneric(text);

        case BTN_SUBMENU:
            if(SubMenu == NULL)
                return -1;
            else
                return this->addSubMenu(text, SubMenu);

        case BTN_ACTION:
            if(act == ACT_NONE)
                return -1;
            else
                return this->addAction(text, act);

        case BTN_TOGGLE:
            return this->addToggle(text, onByDefault);

        case BTN_TEXT:
            return this->addText(text);

        default:
            return -1;
    }
}

 void Menu::removeMenuOption(const int id)
 {
     this->buttons[id] = NULL;
 }

 int Menu::findMenuOptionByText(const char* text)
 {
     for(unsigned int i=0; i < this->buttons.size(); i++)
     {
         if(strncmp(this->buttons[i]->text, text, 
             min(strlen(text), strlen(this->buttons[i]->text))) == 0)
         {
             return this->buttons[i]->id;
         }
     }

     return -1;
 }

void Menu::checkMouseOver(int mouse_x, int mouse_y)
{
    for(unsigned int i=0; i < this->buttons.size(); i++)
    {
        if(detectCollision(this->buttons[i]->mouseOver, createRect(mouse_x, mouse_y, 0, 0)) &&
            (this->buttons[i]->highlight != NULL))
        {
            this->buttons[i]->main = this->buttons[i]->highlight;
        }
        else
        {
            this->buttons[i]->main = this->buttons[i]->normal;
        }
    }
}

int Menu::checkClick(const int x, const int y)
{
    for(unsigned int i=0; i < this->buttons.size(); i++)
    {
        if(detectCollision(this->buttons[i]->mouseOver, createRect(x, y, 0, 0)))
            return i;
    }
    return -1;
}

int Menu::addAction(const char* text, ACTION_TYPE act)
{
    this->buttons.push_back(new Button);

    if(this->center)
    {
        this->buttons[LAST]->normal = renderMultiLineText(
            this->font, text, this->hlColor, this->txtColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlight = renderMultiLineText(
            this->font, text, this->txtColor, this->hlColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->x = ((SCREEN_WIDTH / 2) - 
            (this->buttons[LAST]->normal->clip_rect.w / 2));

        this->offset = this->buttons[LAST]->normal->clip_rect.h;
        this->buttons[LAST]->y = this->y;
        this->y += this->offset;
    }
    else
    {
        this->buttons[LAST]->normal = renderMultiLineText(
            this->font, (string)text, this->hlColor, this->txtColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlight = renderMultiLineText(
            this->font, (string)text, this->txtColor, this->hlColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->offset = this->buttons[LAST]->normal->clip_rect.h;
        this->buttons[LAST]->x = this->x;
        this->buttons[LAST]->y = this->y;
        this->y += this->offset;
    }

    this->buttons[LAST]->mouseOver = createRect(this->buttons[LAST]->x,
        this->buttons[LAST]->y,
        this->buttons[LAST]->normal->clip_rect.w,
        this->buttons[LAST]->normal->clip_rect.h);

    this->buttons[LAST]->btn_type   = BTN_ACTION;
    this->buttons[LAST]->act_type   = act;
    this->buttons[LAST]->text       = new char[strlen(text) + 1];
    strncpy(this->buttons[LAST]->text, text, strlen(text) + 1);

    if(act == ACT_RETURN)
    {
        this->buttons[LAST]->id = -1;
    }
    else if(act == ACT_QUIT)
    {   
        this->buttons[LAST]->id = -2;
    }
    else
    {
        this->buttons[LAST]->id         = this->buttons.size() + 69;
    }

    return this->buttons[LAST]->id;
}

int Menu::addToggle(const char* text, bool on)
{
    this->buttons.push_back(new Button);

    if(this->center)
    {
        this->buttons[LAST]->normal = renderMultiLineText(
            this->font, text, this->hlColor, this->txtColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlight = renderMultiLineText(
            this->font, text, this->txtColor, this->hlColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->x = ((SCREEN_WIDTH / 2) - 
            (this->buttons[LAST]->normal->clip_rect.w / 2));

        this->offset = this->buttons[LAST]->normal->clip_rect.h;
        this->buttons[LAST]->y = this->y;
        this->y += this->offset;
    }
    else
    {
        this->buttons[LAST]->normal = renderMultiLineText(
            this->font, (string)text, this->hlColor, this->txtColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlight = renderMultiLineText(
            this->font, (string)text, this->txtColor, this->hlColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->offset = this->buttons[LAST]->normal->clip_rect.h;
        this->buttons[LAST]->x = this->x;
        this->buttons[LAST]->y = this->y;
        this->y += this->offset;
    }

    this->buttons[LAST]->mouseOver = createRect(this->buttons[LAST]->x,
        this->buttons[LAST]->y,
        this->buttons[LAST]->normal->clip_rect.w,
        this->buttons[LAST]->normal->clip_rect.h);

    this->buttons[LAST]->btn_type       = BTN_TOGGLE;
    this->buttons[LAST]->text           = new char[strlen(text) + 1];
    strncpy(this->buttons[LAST]->text, text, strlen(text) + 1);
    this->buttons[LAST]->act_type       = ACT_NONE;
    this->buttons[LAST]->switch_status  = on;

    this->buttons[LAST]->id             = this->buttons.size() + 69;
    return this->buttons[LAST]->id;
}

int Menu::addSubMenu(const char* text, Menu* nextMenu)
{
    if(nextMenu == NULL)
        return -1;

    this->buttons.push_back(new Button);

    if(this->center)
    {
        this->buttons[LAST]->normal = renderMultiLineText(
            this->font, text, this->hlColor, this->txtColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlight = renderMultiLineText(
            this->font, text, this->txtColor, this->hlColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->x = ((SCREEN_WIDTH / 2) - 
            (this->buttons[LAST]->normal->clip_rect.w / 2));

        this->offset = this->buttons[LAST]->normal->clip_rect.h;
        this->buttons[LAST]->y = this->y;
        this->y += this->offset;
    }
    else
    {
        this->buttons[LAST]->normal = renderMultiLineText(
            this->font, (string)text, this->hlColor, this->txtColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlight = renderMultiLineText(
            this->font, (string)text, this->txtColor, this->hlColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->offset = this->buttons[LAST]->normal->clip_rect.h;
        this->buttons[LAST]->x = this->x;
        this->buttons[LAST]->y = this->y;
        this->y += this->offset;
    }

    this->buttons[LAST]->mouseOver = createRect(this->buttons[LAST]->x,
        this->buttons[LAST]->y,
        this->buttons[LAST]->normal->clip_rect.w,
        this->buttons[LAST]->normal->clip_rect.h);

    this->buttons[LAST]->btn_type   = BTN_SUBMENU;
    this->buttons[LAST]->subMenu    = nextMenu;
    this->buttons[LAST]->act_type   = ACT_NONE;
    this->buttons[LAST]->text       = new char[strlen(text) + 1];
    strncpy(this->buttons[LAST]->text, text, strlen(text) + 1);

    this->buttons[LAST]->id         = this->buttons.size() + 69;
    return this->buttons[LAST]->id;
}

int Menu::addText(const char* text)
{
    this->buttons.push_back(new Button);

    this->buttons[LAST]->btn_type   = BTN_TEXT;
    this->buttons[LAST]->text       = new char[strlen(text) + 1];
    strncpy(this->buttons[LAST]->text, text, strlen(text) + 1);

    if(this->center)
    {
        this->buttons[LAST]->normal = renderMultiLineText(
            this->font, text, this->hlColor, this->txtColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlight = NULL;

        this->buttons[LAST]->x = ((SCREEN_WIDTH / 2) - 
            (this->buttons[LAST]->normal->clip_rect.w / 2));

        this->offset = this->buttons[LAST]->normal->clip_rect.h;
        this->buttons[LAST]->y = this->y;
        this->y += this->offset;
    }
    else
    {
        this->buttons[LAST]->normal = renderMultiLineText(
            this->font, (string)text, this->hlColor, this->txtColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlight = NULL;

        this->offset = this->buttons[LAST]->normal->clip_rect.h;
        this->buttons[LAST]->x = this->x;
        this->buttons[LAST]->y = this->y;
        this->y += this->offset;
    }

    this->buttons[LAST]->mouseOver = createRect(this->buttons[LAST]->x,
        this->buttons[LAST]->y,
        this->buttons[LAST]->normal->clip_rect.w,
        this->buttons[LAST]->normal->clip_rect.h);

    this->buttons[LAST]->btn_type   = BTN_TEXT;
    this->buttons[LAST]->act_type   = ACT_NONE;
    this->buttons[LAST]->text       = new char[strlen(text) + 1];
    strncpy(this->buttons[LAST]->text, text, strlen(text) + 1);

    this->buttons[LAST]->id         = this->buttons.size() + 69;
    return this->buttons[LAST]->id;
}

int Menu::addGeneric(const char* text)
{
    this->buttons.push_back(new Button);

    this->buttons[LAST]->btn_type   = BTN_TEXT;
    this->buttons[LAST]->text       = new char[strlen(text) + 1];
    strncpy(this->buttons[LAST]->text, text, strlen(text) + 1);

    if(this->center)
    {
        this->buttons[LAST]->normal = renderMultiLineText(
            this->font, text, this->hlColor, this->txtColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlight = renderMultiLineText(
            this->font, text, this->txtColor, this->hlColor,
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->x = ((SCREEN_WIDTH / 2) - 
            (this->buttons[LAST]->normal->clip_rect.w / 2));

        this->offset = this->buttons[LAST]->normal->clip_rect.h;
        this->buttons[LAST]->y = this->y;
        this->y += this->offset;
    }
    else
    {
        this->buttons[LAST]->normal = renderMultiLineText(
            this->font, (string)text, this->hlColor, this->txtColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->buttons[LAST]->highlight = renderMultiLineText(
            this->font, (string)text, this->txtColor, this->hlColor,
            TRANSPARENT_BG | CREATE_SURFACE);

        this->offset = this->buttons[LAST]->normal->clip_rect.h;
        this->buttons[LAST]->x = this->x;
        this->buttons[LAST]->y = this->y;
        this->y += this->offset;
    }

    this->buttons[LAST]->mouseOver = createRect(this->buttons[LAST]->x,
        this->buttons[LAST]->y,
        this->buttons[LAST]->normal->clip_rect.w,
        this->buttons[LAST]->normal->clip_rect.h);

    this->buttons[LAST]->btn_type   = BTN_GENERIC;
    this->buttons[LAST]->act_type   = ACT_GENERIC;
    this->buttons[LAST]->text       = new char[strlen(text) + 1];
    strncpy(this->buttons[LAST]->text, text, strlen(text) + 1);

    this->buttons[LAST]->id         = this->buttons.size() + 69;
    return this->buttons[LAST]->id;
}

int Menu::Run()
{
    int mouse_x, mouse_y;
    int status = -1;

    bool clicked;

    if(this->music != NULL)
    {
        if(Mix_PlayMusic(this->music, -1) == -1)
        {
            char error[256];
            sprintf(error, "In-game music disabled!\nError: %s", Mix_GetError());
            handleError(error);
        }
    }

    while(!this->quit)
    {
        this->eventHandler->handleMenuEvents(&this->quit,
                &mouse_x, &mouse_y, &clicked);

        this->checkMouseOver(mouse_x, mouse_y);

        /* Check where the user clicked */
        if(clicked)
            status = this->checkClick(mouse_x, mouse_y);
        else
            status = -1;

        if(status != -1)
        {
            if(this->buttons[status]->btn_type == BTN_SUBMENU)
            {
                int retval = this->buttons[status]->subMenu->Run();
                if(this->buttons[status]->subMenu->findMenuOptionByText("Return") != retval)
                    return retval;
            }

            else if(this->buttons[status]->btn_type == BTN_TOGGLE)
            {
                string tmp(this->buttons[status]->text);

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

                delete[] this->buttons[status]->text;
                this->buttons[status]->text = new char[tmp.length() + 1];
                strncpy(this->buttons[status]->text, tmp.c_str(), tmp.length() + 1);

                if(this->center)
                {
                    this->buttons[status]->normal = renderMultiLineText(
                        this->font, tmp, this->hlColor, this->txtColor,
                        ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

                    this->buttons[status]->highlight = renderMultiLineText(
                        this->font, tmp, this->txtColor, this->hlColor,
                        ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

                    this->buttons[status]->x = ((SCREEN_WIDTH / 2) - 
                        (this->buttons[status]->normal->clip_rect.w / 2));
                }

                else
                {
                    this->buttons[status]->normal = renderMultiLineText(
                        this->font, tmp, this->hlColor, this->txtColor,
                        TRANSPARENT_BG | CREATE_SURFACE);

                    this->buttons[status]->highlight = renderMultiLineText(
                        this->font, tmp, this->txtColor, this->hlColor,
                        TRANSPARENT_BG | CREATE_SURFACE);
                }
            }

            else if(this->buttons[status]->btn_type == BTN_ACTION)
            {
                if(this->buttons[status]->act_type == ACT_QUIT)
                    return -2;
                else if(this->buttons[status]->act_type == ACT_RETURN)
                    return -1;
                else if(this->buttons[status]->act_type == ACT_PLAY)
                {
                    if(this->music != NULL)
                        Mix_HaltMusic();

                    return this->buttons[status]->id;
                }
                else if(this->buttons[status]->act_type == ACT_GENERIC)
                {
                    return this->buttons[status]->id;
                }
            }

            else if(this->buttons[status]->btn_type == BTN_GENERIC)
            {
                return this->buttons[status]->id;
            }

            else if(this->buttons[status]->btn_type == BTN_TEXT)
            {
                /* Do nothing */
            }
        }

        BLIT(this->bg, 0, 0);

        /* Display each menu option */
        for(unsigned int i=0; i < this->buttons.size(); i++)
        {
            BLIT(this->buttons[i]->main,
                this->buttons[i]->x, 
                this->buttons[i]->y);
        }

        /* Update the screen */
        this->display->update();
    }

    /* If we broke out of the while loop because quit was
     * true, we return -1, the ID to quit.
     */
    return -2;
}

int Menu::runNoBlock(const int framesToRunFor)
{
    int count = 0;
    int mouse_x, mouse_y;
    int status;

    bool clicked;

    if(this->music != NULL)
    {
        if(Mix_PlayMusic(this->music, -1) == -1)
        {
            char error[256];
            sprintf(error, "In-game music disabled!\nError: %s", Mix_GetError());
            handleError(error);
        }
    }

    while(!this->quit && count < framesToRunFor)
    {
        count++;

       this->eventHandler->handleMenuEvents(&this->quit,
                &mouse_x, &mouse_y, &clicked);

        this->checkMouseOver(mouse_x, mouse_y);

        /* Check where the user clicked */
        if(clicked)
            status = this->checkClick(mouse_x, mouse_y);
        else
            status = -1;

        if(status != -1)
        {
            if(this->buttons[status]->btn_type == BTN_SUBMENU)
            {
                int retval = this->buttons[status]->subMenu->Run();
                return retval;
            }

            else if(this->buttons[status]->btn_type == BTN_TOGGLE)
            {
                string tmp(this->buttons[status]->text);

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

                delete[] this->buttons[status]->text;
                this->buttons[status]->text = new char[tmp.length() + 1];
                strncpy(this->buttons[status]->text, tmp.c_str(), tmp.length() + 1);

                if(this->center)
                {
                    this->buttons[status]->normal = renderMultiLineText(
                        this->font, tmp, this->hlColor, this->txtColor,
                        ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

                    this->buttons[status]->highlight = renderMultiLineText(
                        this->font, tmp, this->txtColor, this->hlColor,
                        ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

                    this->buttons[status]->x = ((SCREEN_WIDTH / 2) - 
                        (this->buttons[status]->normal->clip_rect.w / 2));
                }

                else
                {
                    this->buttons[status]->normal = renderMultiLineText(
                        this->font, tmp, this->hlColor, this->txtColor,
                        TRANSPARENT_BG | CREATE_SURFACE);

                    this->buttons[status]->highlight = renderMultiLineText(
                        this->font, tmp, this->txtColor, this->hlColor,
                        TRANSPARENT_BG | CREATE_SURFACE);
                }
            }

            else if(this->buttons[status]->btn_type == BTN_ACTION)
            {
                if(this->buttons[status]->act_type == ACT_QUIT)
                    return -2;
                else if(this->buttons[status]->act_type == ACT_RETURN)
                    return -1;
                else if(this->buttons[status]->act_type == ACT_PLAY)
                {
                    if(this->music != NULL)
                        Mix_HaltMusic();

                    return this->buttons[status]->id;
                }
                else if(this->buttons[status]->act_type == ACT_GENERIC)
                {
                    return this->buttons[status]->id;
                }
            }

            else if(this->buttons[status]->btn_type == BTN_GENERIC)
            {
                return this->buttons[status]->id;
            }

            else if(this->buttons[status]->btn_type == BTN_TEXT)
            {
                /* Do nothing */
            }
        }

        BLIT(this->bg, 0, 0);

        /* Display each menu option */
        for(unsigned int i=0; i < this->buttons.size(); i++)
        {
            BLIT(this->buttons[i]->main,
                this->buttons[i]->x, 
                this->buttons[i]->y);
        }

        /* Update the screen */
        this->display->update();
    }

    /* If we broke out of the while loop because quit was
     * true, we return -1, the ID to quit.
     */
    return -2;
}