#include "Engine.h"

vector<string> split(char* str, char token)
{
    vector<string> tokened;
    stringstream   ss;
    string         tmp(str);

    while(getline(ss, tmp, token))
    {
        tokened.push_back(tmp);
    }

    return tokened;
}

Engine::Engine()
{
    this->display       = new Display;
    this->eventHandler  = new Events;
    this->fps           = new Timer;
    this->log           = new Log("NPong.log");

    this->p1            = new Entity(this->display, P1X_START, P1Y_START);
    this->p2            = new Entity(this->display, P2X_START, P2Y_START);
    this->ball          = new Entity(this->display, BALL_X_START, BALL_Y_START);

    this->mainMenu      = new Menu(this->display, this->eventHandler);
    this->mpMenu        = new Menu(this->display, this->eventHandler);

    this->ball          ->setEntity(this->display->loadAlphaImage("Data"FN_SLASH"Ball.png"));
	this->ball          ->setCollisionBox(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 9, 9);

	this->p1            ->createSurface(PONG_WIDTH, PONG_HEIGHT, WHITE, 0);
	this->p1            ->setCollisionBox(P1X_START, P1Y_START, PONG_HEIGHT + 2, PONG_WIDTH);

	this->p2            ->createSurface(PONG_WIDTH, PONG_HEIGHT, WHITE, 0);
	this->p2            ->setCollisionBox(P2X_START, P2Y_START, PONG_HEIGHT + 2, PONG_WIDTH);

    this->p1_score      = 0;
    this->p2_score      = 0;
    this->score_font    = TTF_OpenFont("Data"FN_SLASH"MainFont.ttf", 24);

    bg                  = createSurface(SCREEN_WIDTH, SCREEN_HEIGHT, createColor(BLACK));

    this->fps->setFrameRate(FPS);

#ifdef _DEBUG
    LOG("Constructor complete", DEBUG);
#endif // _DEBUG
}

Engine::~Engine()
{
    delete this->display;
    delete this->eventHandler;
    delete this->fps;

    delete this->p1;
    delete this->p2;
    delete this->ball;

    delete this->mainMenu;
    delete this->mpMenu;

    TTF_CloseFont(this->score_font);

    SDL_FreeSurface(this->bg);

#ifdef _DEBUG
    LOG("Destructor complete", DEBUG);
#endif // _DEBUG

    LOG("Game over!", INFO);

    logger << "The final score was " << this->p1_score << " to " << this->p2_score;
    LOG(logger.str(), INFO);
    logger.str(string());

    delete this->log;
}

void Engine::menu()
{
    LOG("Loading game font...", INFO);
    /* The menu font */
    TTF_Font* menu_font = TTF_OpenFont("Data"FN_SLASH"MainFont.ttf", 24);

#ifdef _DEBUG
    LOG("Creating menus...", DEBUG);
#endif // _DEBUG

    /* Set up the menus, giving them a
     * -font
     * -background image
     * -text color
     * -highlight color
     * -starting coordinates
     */
    this->mainMenu->setBackground(createSurface(
        SCREEN_WIDTH, SCREEN_HEIGHT, createColor(BLACK)));
    this->mainMenu->setFont(menu_font);
    this->mainMenu->setTextColor(WHITE);
    this->mainMenu->setHighLightColor(GREEN);
    this->mainMenu->setStartCoordinates(SCREEN_WIDTH / 2, 100);
    this->mainMenu->setCenterText(true);

    /* A new menu for multiplayer so the user can
     * choose whether they would like to host a server
     * or join a game.
     */

    this->mpMenu->setBackground(createSurface(
        SCREEN_WIDTH, SCREEN_HEIGHT, createColor(BLACK)));
    this->mpMenu->setFont(menu_font);
    this->mpMenu->setTextColor(WHITE);
    this->mpMenu->setHighLightColor(GREEN);
    this->mpMenu->setStartCoordinates(SCREEN_WIDTH / 2, 100);
    this->mpMenu->setCenterText(true);

    /* The second arg is irrelevant here */
    this->mpMenu->newText("PONG!\n \nCreated By George Kudrayvtsev\n ");
    this->mpMenu->newActionOption("Host a Game", PLAY_GAME);
    this->mpMenu->newActionOption("Manually Enter a Server", PLAY_MULTI_GAME);
    this->mpMenu->newActionOption("Scan For Games", LOAD_GAME);
    this->mpMenu->newActionOption("Return", RETURN_TO_LAST);  // Except in this one

    /* A menu with the controls to help out the user.
     * It has only one option, to return.
     */
    Menu* instructions = new Menu(this->display, this->eventHandler);
    instructions->setBackground(createSurface(
        SCREEN_WIDTH, SCREEN_HEIGHT, createColor(BLACK)));
    instructions->setFont(menu_font);
    instructions->setTextColor(WHITE);
    instructions->setHighLightColor(GREEN);
    instructions->setStartCoordinates(SCREEN_WIDTH / 2, 100);
    instructions->setCenterText(true);

    instructions->newText("PONG!\n \nCreated By George Kudrayvtsev");
    instructions->newText("\nCONTROLS:\nF: Toggle fullscreen\nUp Arrow: Move paddle up\nDown Arrow: Move paddle down\nEscape: Quit Game\n ");
    instructions->newActionOption("Return", RETURN_TO_LAST);

    /* Add all of the main menu options */
    this->mainMenu->newText("PONG!\n \nCreated By George Kudrayvtsev\n ");
    this->mainMenu->newActionOption("Singleplayer Game", PLAY_GAME);
    this->mainMenu->newActionOption("Multiplayer Game", PLAY_MULTI_GAME);
    this->mainMenu->newSubMenu("Controls", instructions);
    this->mainMenu->newActionOption("Quit", QUIT_GAME);

    /* Run the menu, checking for a return value */
    LOG("Main menu!", INFO);
    int ret = this->mainMenu->run();
    if(ret == -1)   // -1 means quit
        return;
    else if(ret == 0)   // 0 means play single
        this->playGame();
    else if(ret == 2)   // 2 means play multi, enter host
        this->setupMulti();
    else if(ret == 3)   // 3 means play multi, scan network
        this->setupMulti(true);
    else    // Some error?
        handleError("The main menu returned an undefined value!", false);
}

void Engine::playGame()
{
    /* Know when to exit */
    bool quit = false;

    /* Game variables */
    int p1_dy   = 0;
    int p2_dy   = 0;

    int ball_dx = 8;
    int ball_dy = -8;

    /* Handling the score */
    stringstream score_ss;
    score_ss << this->p1_score << "    |    " << this->p2_score;

    SDL_Surface* score = renderMultiLineText(this->score_font, 
        score_ss.str(), createColor(RED), createColor(WHITE),
        CREATE_SURFACE | ALIGN_CENTER | TRANSPARENT_BG);

    /* We want to update AI move every 10 frames */
    int frame = 0;

    bool change = false;

    while(!quit)
    {
        /* If frame is greater than 10, make it 0, otherwise increment by 1 */
        frame > 10 ? frame = 0 : frame++;

        /* Handle user interaction */
        this->eventHandler->handleGameEvents(&quit, &p1_dy, &change);

        if(change)
        {
            this->display->toggleFullscreen();
            change = false;
        }

        /* Clear the score */
        SDL_FreeSurface(score);
        score_ss.str(string());

        /* Update the score */
        score_ss << this->p1_score << "    |    " << this->p2_score;
        SDL_Surface* score = renderMultiLineText(this->score_font, 
            score_ss.str(), createColor(RED), createColor(WHITE),
            CREATE_SURFACE | ALIGN_CENTER | TRANSPARENT_BG);

        /* Get the AI path */
        if(frame % 10 == 0)
            this->calcMove(&p2_dy, ball_dx, ball_dy);

        /* Move both paddles */
        this->p1->move(this->p1->getX(), this->p1->getY() + p1_dy);
		this->p2->move(this->p2->getX(), this->p2->getY() + p2_dy);

        /* Check for paddle collision with borders */
		if(this->p2->getY() + PONG_HEIGHT >= SCREEN_HEIGHT)
		{
			p2_dy = 0;
			this->p2->move(this->p2->getX(), SCREEN_HEIGHT - PONG_HEIGHT - 1);
		}
		else if(this->p2->getY() <= 0)
		{
			p2_dy = 0;
			this->p2->move(this->p2->getX(), 1);
		}

		if(this->p1->getY() + PONG_HEIGHT >= SCREEN_HEIGHT)
		{
			p1_dy = 0;
			this->p1->move(this->p1->getX(), SCREEN_HEIGHT - PONG_HEIGHT - 1);
		}
		else if(this->p1->getY() <= 0)
		{
			p1_dy = 0;
			this->p1->move(this->p1->getX(), 1);
		}

        /* Update scores if goal */
		if(this->ball->getX() + 1 + this->p2->getCollisionBox()->w >= SCREEN_WIDTH)
		{
            LOG("Player 1 has scored!", INFO);
			this->p1_score++;
            logger << "The score is now " << this->p1_score << " to " << this->p2_score;
            LOG(logger.str(), INFO);
            logger.str(string());
		}
		else if(this->ball->getX() <= 0)
        {
            LOG("Player 2 has scored!", INFO);
			this->p2_score++;
            logger << "The score is now " << this->p1_score << " to " << this->p2_score;
            LOG(logger.str(), INFO);
            logger.str(string());
        }

        /* Reset ball to middle if scored*/
		if(this->ball->getX() <= 0 || 
			this->ball->getX() + this->p2->getCollisionBox()->w >= SCREEN_WIDTH)
		{
			srand((unsigned int)time(NULL));

            int pos_neg = rand() % 1;

            if(pos_neg == 1)
            {
                ball_dx = -(rand() % 8 + 8);
            }
            else
            {
                ball_dx = (rand() % 8 + 8);
            }

            pos_neg = rand() % 1;


            if(pos_neg == 1)
            {
                ball_dy = -(rand() % 8 + 8);
            }
            else
            {
                ball_dy = (rand() % 8 + 8);
            }

            this->ball->move(SCREEN_HEIGHT / 2 - 5, SCREEN_WIDTH / 2 - 5);
		}

        /* Change direction of ball on impact with paddle or walls */
        if(this->ball->getY() <= 0 + this->ball->getCollisionBox()->h ||
            this->ball->getY() >= SCREEN_HEIGHT - this->ball->getCollisionBox()->h)
		{
			ball_dy = -(ball_dy);
		}

        /* Did the ball hit either paddle? */
	    if(detectCollision(*this->ball->getCollisionBox(), *this->p1->getCollisionBox()) || 
		    detectCollision(*this->ball->getCollisionBox(), *this->p2->getCollisionBox()))
	    {
		    ball_dy = ball_dy;
		    ball_dx = -(ball_dx);
	    }

        /* Update the ball's position */
        this->ball->move(this->ball->getX() + ball_dx, this->ball->getY() + ball_dy);

        /* Update all images on the screen */
        BLIT(bg, 0, 0);
        BLIT(score, SCREEN_WIDTH / 2 - score->clip_rect.w / 2, 0);
        this->p1->blit();
        this->p2->blit();
        this->ball->blit();
        this->display->update();

        /* Cap frame rate */
        this->delayFps();
    }
}

void Engine::setupMulti(bool scanNetwork)
{
#ifdef _DEBUG
    LOG("Host or Join?", DEBUG);
#endif // _DEBUG

    /* Run the multiplayer menu */
    if(!scanNetwork)
    {
        int retval = this->mpMenu->run();

        if(retval == 1) // Return to previous (main) menu
            this->menu();
        else if(retval == 0)    // Host
        {
            /* If the user wants to host a game,
             * NPong will create a socket and accept
             * a connection on the default port, 2012.
             * The host controls ball movement and the
             * score, the client only controls his or
             * her own paddle.
             */
    #ifdef _DEBUG
            LOG("Host", DEBUG);
    #endif // _DEBUG

            this->isHost = true;
            this->peer = new Socket(AF_INET, SOCK_STREAM, TCP_SERVER);
            this->peer->bind("", DEF_PONG_PORT);
            this->peer->listen(1);

            /* To handle exiting */
            bool quit = false;

            /* So the user knows what's going on */
            SDL_Surface* status = renderMultiLineText(
                this->score_font, "Awaiting an opponent...",
                createColor(BLACK), createColor(WHITE), 
                CREATE_SURFACE | ALIGN_CENTER);

            while(!quit)
            {
                this->eventHandler->handleQuit(&quit);

                /* Update the images and text on the screen */
                BLIT(this->bg, 0, 0);
                BLIT(status, SCREEN_WIDTH / 2 - status->clip_rect.w / 2, 100);
                this->display->update();

                /* Await a connection. It is non-blocking so the
                 * user can now safely exit.
                 */
                int opponent = this->peer->nonBlockAccept();

                if(opponent != -1)
                {
                    break;
                }

                /* Lower CPU usage */
                SDL_Delay(10);
            }

            if(!quit)
            {
    #ifdef _DEBUG
                /* Log the socket file descriptor so it's not an error */
                logger << "Socket FD: " << this->peer->getClientSock();
                LOG(logger.str(), DEBUG);
                logger.str(string());
    #endif // _DEBUG

                /* Get the confirmation of connection message from the opponent */
                char* msg = NULL;
                do
                {
                    msg = this->peer->recv();
    #ifdef _DEBUG
                    LOG(msg, DEBUG);
    #endif // _DEBUG

                }
                while(strncmp(msg, "CONNECT\n", 8) != 0);

                /* Just show the user that someone is
                 * ready to play.
                 * TODO: Add a confirmation message so incase
                 * user is AFK they can be informed.
                 */

    #ifdef _DEBUG
                LOG("Peer sent \"CONNECT\\n\" message!", DEBUG);
    #endif // _DEBUG

                SDL_FreeSurface(status);
                status = renderMultiLineText(
                    this->score_font, "Starting match...",
                    createColor(BLACK), createColor(WHITE), 
                    CREATE_SURFACE | ALIGN_CENTER);

                /* Put surfaces onto screen */
                BLIT(this->bg, 0, 0);
                BLIT(status, SCREEN_WIDTH / 2 - status->clip_rect.w / 2, 100);

                /* Update screen */
                this->display->update();

                /* We have received a connect message, so we can now play. */
                this->playMultiGame();
            }

        }
        else if(retval == 2)    // Join
        {
            /* Join a server that has been created on the LAN.
             * Currently does not connect over the Internet.
             * FIXME: Make NPong work over the Internet.
             */
    #ifdef _DEBUG
            LOG("Join", DEBUG);
            LOG("Prompting for IP...", DEBUG);
    #endif // _DEBUG

            /* We want to join, not host */
            this->isHost = false;

            /* Instructions */
            SDL_Surface* main_line = renderMultiLineText(this->score_font,
                "Enter the server IP address:", createColor(BLACK), createColor(WHITE),
                ALIGN_CENTER | CREATE_SURFACE | TRANSPARENT_BG);

            /* Quitting var */
            bool quit = false;

            /* Surface to show what user types */
            SDL_Surface* ip_surf = renderMultiLineText(this->score_font,
                " ", createColor(BLACK), createColor(WHITE),
                ALIGN_CENTER | CREATE_SURFACE | TRANSPARENT_BG);

            SDL_Event evt;
            string ip = "";

            /* Needed to get char from keyboard */
            SDL_EnableUNICODE(SDL_ENABLE); 

            while(!quit)
            {
                SDL_FreeSurface(ip_surf);
                ip_surf = renderMultiLineText(this->score_font, ip, 
                    createColor(BLACK), createColor(WHITE),
                    ALIGN_CENTER | CREATE_SURFACE | TRANSPARENT_BG);

                if(SDL_PollEvent(&evt))
                {
                    switch(evt.type)
                    {
                    case SDL_QUIT:
                        quit = true;
                        break;
                    case SDL_KEYDOWN:
                        /* Only numbers and periods for IP addresses */
                        if((evt.key.keysym.unicode >= (Uint16)'0') &&
                            (evt.key.keysym.unicode <= (Uint16)'9'))
                        {
                            ip += (char)evt.key.keysym.unicode;
                        }
                        else if(evt.key.keysym.sym == SDLK_BACKSPACE)
                        {
                            ip = ip.substr(0, ip.length() - 1);
                        }
                        else if(evt.key.keysym.unicode == (Uint16)'.')
                        {
                            ip += (char)evt.key.keysym.unicode;
                        }
                        break;
                    }
                }
                if(evt.key.keysym.sym == SDLK_RETURN)   // Done
                {
                    break;
                }

                /* Put all surfaces on the screen */
                BLIT(this->bg, 0, 0);
                BLIT(main_line, SCREEN_WIDTH / 2 - main_line->clip_rect.w / 2, 100);
                BLIT(ip_surf, SCREEN_WIDTH / 2 - ip_surf->clip_rect.w / 2, 200);

                /* Update everything for the user to see */
                this->display->update();

#ifdef _WIN32
                Sleep(10);
#else
                sleep(10);
#endif // _WIN32
            }

            /* Unicode isn't needed anymore */
            SDL_EnableUNICODE(SDL_DISABLE);

            /* As long as there something entered and user
             * didn't want to quit, we have to create a socket
             * and connect to the IP. Error handling is done
             * by Socket::connect(). Once connected we send 
             * "CONNECT\n" to the server to show we are available.
             */
            if(ip != "" && !quit)
            {
    #ifdef _DEBUG
                logger << "The IP address is: " << ip;
                LOG(logger.str(), DEBUG);
                logger.str(string());
    #endif // __DEBUG

                this->peer = new Socket(AF_INET, SOCK_STREAM);
                this->peer->connect(ip.c_str(), DEF_PONG_PORT);
                this->peer->sendall("CONNECT\n");
                this->playMultiGame();
            }
        }
    }
    else
    {
        bool quit = false;

        SDL_Surface* scan = renderMultiLineText(this->score_font, 
            "Scanning for games...", createColor(BLACK), createColor(WHITE),
            ALIGN_CENTER | TRANSPARENT_BG | CREATE_SURFACE);

        while(!quit)
        {
            this->eventHandler->handleQuit(&quit);
            BLIT(scan, SCREEN_WIDTH / 2 - scan->clip_rect.w / 2, 100);
            this->display->update();
        }
    }
}

void Engine::playMultiGame()
{
    /* Know when to exit */
    bool quit = false;

    /* Game variables */
    int p1_dy   = 0;
    int p2_dy   = 0;

    int p2_y    = P1X_START;

    int ball_dx = 5;
    int ball_dy = -5;

    /* Handling the score */
    stringstream score_ss;
    score_ss << this->p1_score << "    |    " << this->p2_score;
    this->score_font = TTF_OpenFont("Data"FN_SLASH"MainFont.ttf", 24);
    SDL_Surface* score = renderMultiLineText(this->score_font, 
        score_ss.str(), createColor(RED), createColor(WHITE),
        CREATE_SURFACE | ALIGN_CENTER | TRANSPARENT_BG);

    LOG("Starting multiplayer game...", INFO);

    bool change = false;

    while(!quit)
    {
        /* Handle user interaction */
        this->eventHandler->handleGameEvents(&quit, &p1_dy, &change);

        if(change)
        {
            this->display->toggleFullscreen();
            change = false;
        }

        /* Clear the score */
        SDL_FreeSurface(score);
        score_ss.str(string());

        /* Update the score */
        score_ss << this->p1_score << "    |    " << this->p2_score;
        SDL_Surface* score = renderMultiLineText(this->score_font, 
            score_ss.str(), createColor(RED), createColor(WHITE),
            CREATE_SURFACE | ALIGN_CENTER | TRANSPARENT_BG);

        /* Update movement */
        this->p1->move(this->p1->getX(), this->p1->getY() + p1_dy);

        /* Send data to other player */
        this->sendPosition();
        this->recvPosition(&p2_y);

        this->p2->move(this->p2->getX(), p2_y);

        if(this->isHost)
        {
            /* Check for paddle collision with borders */
		    if(this->p2->getY() + PONG_HEIGHT >= SCREEN_HEIGHT)
		    {
			    p2_dy = 0;
			    this->p2->move(this->p2->getX(), this->p2->getY() - p2_dy);
		    }
		    else if(this->p2->getY() <= 0)
		    {
			    p2_dy = 0;
			    this->p2->move(this->p2->getX(), this->p2->getY() + p2_dy);
		    }

		    if(this->p1->getY() + PONG_HEIGHT >= SCREEN_HEIGHT)
		    {
			    p1_dy = 0;
			    this->p1->move(this->p1->getX(), SCREEN_HEIGHT - PONG_HEIGHT - 1);
		    }
		    else if(this->p1->getY() <= 0)
		    {
			    p1_dy = 0;
			    this->p1->move(this->p1->getX(), 1);
		    }

            /* Update scores if goal */
		    if(this->ball->getX() + 1 + this->p2->getCollisionBox()->w >= SCREEN_WIDTH)
		    {
                LOG("Player 1 scored!", INFO);
			    this->p1_score++;
                logger << "The score is now " << this->p1_score << " to " << this->p2_score;
                LOG(logger.str(), INFO);
                logger.str(string());
		    }
		    else if(this->ball->getX() <= 0)
            {
                LOG("Player 2 scored!", INFO);
			    this->p2_score++;
                logger << "The score is now " << this->p1_score << " to " << this->p2_score;
                LOG(logger.str(), INFO);
                logger.str(string());
            }

            /* Reset ball to middle if scored*/
		    if(this->ball->getX() <= 0 || 
			    this->ball->getX() + this->p2->getCollisionBox()->w >= SCREEN_WIDTH)
		    {
			    srand((unsigned int)time(NULL));

			    ball_dx = -5;
			    ball_dy = (rand() % 5) - 1;

			    if(ball_dy == 0)
				    ball_dy += 1 + rand() % 5;

                this->ball->move(SCREEN_HEIGHT / 2 - 5, SCREEN_WIDTH / 2 - 5);
		    }

            /* Change direction of ball on impact with paddle or walls */
            if(this->ball->getY() <= 0 + this->ball->getCollisionBox()->h ||
                this->ball->getY() >= SCREEN_HEIGHT - this->ball->getCollisionBox()->h)
		    {
			    ball_dy = -(ball_dy);
		    }

            /* Did the ball hit either paddle? */
		    if(detectCollision(*this->ball->getCollisionBox(), *this->p1->getCollisionBox()) || 
			    detectCollision(*this->ball->getCollisionBox(), *this->p2->getCollisionBox()))
		    {
			    ball_dy = ball_dy;
			    ball_dx = -(ball_dx);
		    }

            /* Update the ball's position */
            this->ball->move(this->ball->getX() + ball_dx, this->ball->getY() + ball_dy);
        } // if(isHost())

        /* Update all images on the screen */
        BLIT(bg, 0, 0);
        BLIT(score, SCREEN_WIDTH / 2 - score->clip_rect.w / 2, 0);
        this->p1->blit();
        this->p2->blit();
        this->ball->blit();
        this->display->update();

        /* Cap frame rate */
        this->delayFps();
    }
}

void Engine::sendPosition()
{
    stringstream ss;
    ss << "X:" << this->p1->getX();
    ss << "Y:" << this->p1->getY();

    if(this->isHost)
    {
        /* Only the host can control ball position and score.
         * This (hopefully?) prevents cheating unless the 
         * cracker forces host.
         */
        ss << "P1SCORE:" << this->p1_score;
        ss << "P2SCORE:" << this->p2_score;

        if(this->ball->getX() > SCREEN_WIDTH / 2)
            ss << "BALLX:" << SCREEN_WIDTH / 2 - (this->ball->getX() - (SCREEN_WIDTH / 2));
        else
            ss << "BALLX:" << SCREEN_WIDTH - this->ball->getX();

        ss << "BALLY:" << this->ball->getY();
    }

    ss << "\n";

#ifdef _DEBUG
    /* Do some logging for debug purposes */
    logger << "Sent " << sizeof ss.str() << " bytes.";
    LOG(logger.str(), DEBUG);
    logger.str(string());
    logger << "Sending coordinates: " << ss.str();
    LOG(logger.str(), DEBUG);
    logger.str(string());
#endif // _DEBUG

    /* Send the complete message, in the format
     * X:x-coorY:y-coorP1SCORE:player1-score:P2SCORE:player2-scoreBALLX:ball-xBALLY:ball-y\n
     */
    this->peer->sendall(ss.str().c_str());
}

void Engine::recvPosition(int* y)
{
    /* Get data from the socket */
    string msg(this->peer->recv());

    /* No data? */
    if(msg.empty())
        return;

    /* Corrupt data? */
    if(msg.find("X:") == string::npos || 
        msg.find("X:") == -1)
        return;

#ifdef _DEBUG
    /* Logging to debug easily */
    logger << "Received data from peer: " << msg;
    LOG(logger.str(), DEBUG);
    logger.str(string());
#endif // _DEBUG

    /* All variables to parse:
     * paddle_x, paddle_y, ball_x,
     * ball_y, player1_score, player2_score
     */
    int px, py, bx, by;
    int p1sc, p2sc;

    /* Parse the paddle coordinates from the entire packet */
    px  = atoi(msg.substr(msg.find("X:") + 2, msg.find("Y:")).c_str());
    py  = atoi(msg.substr(msg.find("Y:") + 2, msg.find("P1SCORE:")).c_str());

    if(!this->isHost)
    {
        /* The client must use host data */
        p1sc= atoi(msg.substr(msg.find("P1SCORE:") + 8, msg.find("P2SCORE:")).c_str());
        p2sc= atoi(msg.substr(msg.find("P2SCORE:") + 8, msg.find("BALLX:")).c_str());
        bx  = atoi(msg.substr(msg.find("BALLX:") + 6, msg.find("BALLY:")).c_str());
        by  = atoi(msg.substr(msg.find("BALLY:") + 6, msg.find("\n")).c_str());
        this->ball->move(bx, by);

        this->p1_score = p1sc;
        this->p2_score = p2sc;
    }

    /* Set the y of the paddele for player 2 (host) */
    *y  = py;
}

void Engine::delayFps()
{
    /* Calculate frame rate and how much to pause in order
     * to keep a constant frame rate.
     */
#ifdef _DEBUG
    logger << "Ticks: " << this->fps->getTicks();
    logger << " FPS: " << this->fps->FRAME_RATE;
    logger << " Calc: " << 1000 / this->fps->FRAME_RATE;
    LOG(logger.str(), DEBUG);
    logger.str(string());
#endif // _DEBUG

    if(((unsigned)this->fps->getTicks() < 1000 / this->fps->FRAME_RATE))
    {
        int delay = (1000 / this->fps->FRAME_RATE) - this->fps->getTicks();

#ifdef _DEBUG
        logger << "Delaying for " << delay << "ms";
        LOG(logger.str(), DEBUG);
        logger.str(string());
#endif // _DEBUG

        SDL_Delay(delay);
    }
}

void Engine::calcMove(int* ai_dy, const int dx, const int dy)
{
	/*
		*ai_dy = an int to move the paddle
		x      = where the ball is (x)
		y      = where the ball is (y)
		dx     = rate at which ball is moving (x)
		dy     = rate at which ball is moving (y)
	*/

    /* Temporary variables */
    int x             = this->ball->getX();
    int y             = this->ball->getY();

	int impact_y      = y + (dy*((SCREEN_WIDTH - x) / dx));

    /* We need to estimate impact so a temporary 
     * variable is necessary.
     */
	int tmp           = abs(impact_y);

#ifdef _DEBUG
    /* Log to debug and inform */
    logger << "Estimated impact: " << impact_y;
    LOG(logger.str(), DEBUG);
    logger.str(string());
#endif // _DEBUG

    /* If the ball is going away from the AI paddle,
     * we need to move back toward the middle for
     * maximum effectiveness on the next turn.
     */
	if(dx < 0)  // Going to the left
	{
		if(this->p2->getY() < SCREEN_HEIGHT / 2)    // Above the middle
		{
			if(this->p2->getY() + 40 >= SCREEN_HEIGHT / 2 && this->p2->getY() <= SCREEN_HEIGHT / 2)
			{
				*ai_dy = 0;
				return;
			}
			else
				*ai_dy = 5;
		}
		else if(this->p2->getY() > SCREEN_HEIGHT / 2)   // Below the middle
		{
			if(this->p2->getY() - 40 <= SCREEN_HEIGHT / 2 && this->p2->getY() >= SCREEN_HEIGHT / 2)
			{
				*ai_dy = 0;
				return;
			}
			else
				*ai_dy = -5;
		}
		else    // Exactly at the middle
			*ai_dy = 0;

		return;
	}

    /* Here we estimate the position necessary
     * for the ball to hit the paddle in the 
     * middle. The seemingly arbitrary "40" in
     * there is actually the height of the paddle.
     */
	if(dy < 0) // Going up
	{
		int min = this->p2->getY();
		int max = this->p2->getY() + 40;
		if(tmp < max && tmp > min)
		{
			*ai_dy = 0;
			return;
		}
	}
	else if(dy > 0) // Going down
	{
		int max = this->p2->getY() + 40;
		int min = this->p2->getY();
		if(tmp < max && tmp > min)
		{
			*ai_dy = 0;
			return;
		}
	}

    /* If the impact is outside of the screen border */
	if(impact_y > SCREEN_HEIGHT)
	{
		impact_y = SCREEN_HEIGHT - (impact_y - SCREEN_HEIGHT);
	}
	if(impact_y < 0)
	{
		impact_y = 0 - impact_y;
	}

    /* Default action, if impact is above
     * paddle, go up, else if impact is
     * below, go down, otherwise just
     * stay still.
     */
	if(this->p2->getY() > impact_y)
		*ai_dy = -5;
	else if(this->p2->getY() < impact_y)
		*ai_dy = 5;
	else
		return;
}