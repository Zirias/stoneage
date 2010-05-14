#include "entity.h"

SDL_Rect drawArea;

static void
m_init ARG(Board b, int x, int y)
{
    METHOD(Entity);

    this->b = b;
    this->x = x;
    this->y = y;
}

static void
m_draw ARG()
{
    METHOD(Entity);

    SDL_Surface *screen;

    drawArea.w = this->tile->w;
    drawArea.h = this->tile->h;
    drawArea.x = this->x * drawArea.w;
    drawArea.y = this->y * drawArea.h;

    screen = this->b->getScreen(this->b);
    SDL_BlitSurface(this->tile, NULL, screen, &drawArea);
    SDL_UpdateRects(screen, 1, &drawArea);
}

static void
m_dispose ARG()
{
    METHOD(Entity);
    DELETE(Entity, this);
}

CTOR(Entity)
{
    BASECTOR(Entity, Object);
    this->tile = NULL;
    this->init = &m_init;
    this->draw = &m_draw;
    this->dispose = &m_dispose;
    return this;
}

DTOR(Entity)
{
    BASEDTOR(Object);
}

