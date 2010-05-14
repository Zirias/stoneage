#include "entity.h"

static SDL_Rect drawArea;

static void
m_init ARG(Board b, int x, int y)
{
    METHOD(Entity);

    this->b = b;
    this->x = x;
    this->y = y;
}

static void
m_draw ARG(int refresh)
{
    METHOD(Entity);

    SDL_Surface *screen, *tile;

    tile = this->getSurface(this->b);

    drawArea.w = tile->w;
    drawArea.h = tile->h;
    drawArea.x = this->x * drawArea.w;
    drawArea.y = this->y * drawArea.h;

    screen = SDL_GetVideoSurface();

    SDL_BlitSurface(tile, NULL, screen, &drawArea);
    if (refresh) SDL_UpdateRects(screen, 1, &drawArea);
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
    this->init = &m_init;
    this->draw = &m_draw;
    this->dispose = &m_dispose;
    return this;
}

DTOR(Entity)
{
    BASEDTOR(Object);
}

