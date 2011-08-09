#ifndef ENTITY_H
#define ENTITY_H

#include "Display.h"
#include "SDL_Functions.h"

class Entity
{
    /*
        Base entity class with the most low-level functionality
        such as handling colors and collisions.
    */
public:
    Entity(Display* display, const int initial_x, const int initial_y);
    ~Entity();

    void setEntity(SDL_Surface* entity);
    void loadEntity(char* filename);
    void freeEntity();
    void createSurface(const int w, const int h, const Uint32 r, const Uint32 g, const Uint32 b, const Uint32 a);
    void fillRect(const int x, const int y, const int w, const int h, const Uint32 color);
    void move(const int x, const int y);
    void blit();
    void setCollisionBox(const int x, const int y, const int h, const int w);
    SDL_Surface* getEntity();
    int getX(){return this->x;}
    int getY(){return this->y;}
    const SDL_Rect* getCollisionBox();

protected:

    Display*        display;
    SDL_Surface*    main_entity;
    SDL_Rect        collision_box;

    int x;
    int y;
};

#endif // ENTITY_H