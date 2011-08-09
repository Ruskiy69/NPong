#include "Entity.h"

Entity::Entity(Display* display, const int initial_x, const int initial_y)
{
    this->display = display;
    this->x       = initial_x;
    this->y       = initial_y;
}

Entity::~Entity()
{
}

void Entity::setEntity(SDL_Surface* entity)
{
    this->main_entity = entity;
}

void Entity::loadEntity(char* filename)
{
    this->main_entity = this->display->loadAlphaImage(filename);
}

void Entity::freeEntity()
{
    SDL_FreeSurface(this->main_entity);
}

void Entity::createSurface(const int w, const int h, const Uint32 r, const Uint32 g, const Uint32 b, const Uint32 a)
{
    this->main_entity = ::createSurface(w, h, createColor(r, g, b), SDL_HWSURFACE);
}

void Entity::move(const int x, const int y)
{
    this->x = x;
    this->y = y;

    this->collision_box.x = this->x;
    this->collision_box.y = this->y;

    BLIT(this->main_entity, this->x, this->y);
}

void Entity::blit()
{
    BLIT(this->main_entity, this->x, this->y);
}

void Entity::setCollisionBox(const int x, const int y, const int h, const int w)
{
    this->collision_box.x = x;
    this->collision_box.y = y;
    this->collision_box.h = h;
    this->collision_box.w = w;
}

SDL_Surface* Entity::getEntity()
{
    return this->main_entity;
}

const SDL_Rect* Entity::getCollisionBox()
{
    return &this->collision_box;
}
