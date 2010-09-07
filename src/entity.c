#include "entity.h"
#include "screen.h"
#include "board.h"

static const int emptyTileTable[16][2] = {
    {SATN_Empty, 0},
    {SATN_Empty_1, 3},
    {SATN_Empty_1, 2},
    {SATN_Empty_2a, 2},
    {SATN_Empty_1, 1},
    {SATN_Empty_2f, 1},
    {SATN_Empty_2a, 1},
    {SATN_Empty_3, 1},
    {SATN_Empty_1, 0},
    {SATN_Empty_2a, 3},
    {SATN_Empty_2f, 0},
    {SATN_Empty_3, 2},
    {SATN_Empty_2a, 0},
    {SATN_Empty_3, 3},
    {SATN_Empty_3, 0},
    {SATN_Empty_4, 0},
};

static void
m_init(THIS, Board b, int x, int y)
{
    METHOD(Entity);

    this->b = b;
    this->x = x;
    this->y = y;
    this->moving = 0;

    this->MoveStarting = CreateEvent();
    this->PositionChanged = CreateEvent();
}

static unsigned int
calculateNeighbors(Entity this)
{
    unsigned int neighbors = 0;
    Entity neighbor;

    if (this->y > 0)
    {
	this->b->entity(this->b, this->x, this->y - 1, &neighbor);
	if (neighbor && neighbor->bg == BG_Earth) neighbors += 8;
    }

    if (this->x < LVL_COLS-1)
    {
	this->b->entity(this->b, this->x + 1, this->y, &neighbor);
	if (neighbor && neighbor->bg == BG_Earth) neighbors += 4;
    }

    if (this->y < LVL_ROWS-1)
    {
	this->b->entity(this->b, this->x, this->y + 1, &neighbor);
	if (neighbor && neighbor->bg == BG_Earth) neighbors += 2;
    }

    if (this->x > 0)
    {
	this->b->entity(this->b, this->x - 1, this->y, &neighbor);
	if (neighbor && neighbor->bg == BG_Earth) neighbors += 1;
    }

    if (!(neighbors&(8|4)) && this->y > 0 && this->x < LVL_COLS-1)
    {
	this->b->entity(this->b, this->x + 1, this->y - 1, &neighbor);
	if (neighbor && neighbor->bg == BG_Earth) neighbors += 128;
    }

    if (!(neighbors&(4|2)) && this->x < LVL_COLS-1 && this->y < LVL_ROWS-1)
    {
	this->b->entity(this->b, this->x + 1, this->y + 1, &neighbor);
	if (neighbor && neighbor->bg == BG_Earth) neighbors += 64;
    }

    if (!(neighbors&(2|1)) && this->y < LVL_ROWS - 1 && this->x > 0)
    {
	this->b->entity(this->b, this->x - 1, this->y + 1, &neighbor);
	if (neighbor && neighbor->bg == BG_Earth) neighbors += 32;
    }

    if (!(neighbors&(8|1)) && this->x > 0 && this->y > 0)
    {
	this->b->entity(this->b, this->x - 1, this->y - 1, &neighbor);
	if (neighbor && neighbor->bg == BG_Earth) neighbors += 16;
    }

    return neighbors;
}

static const SDL_Surface *
m_getTile(THIS)
{
    return 0;
}

static void
m_draw(THIS, int refresh)
{
    METHOD(Entity);

    SDL_Rect drawArea;
    const SDL_Surface *tile;
    Screen s;
    SDL_Surface *sf;

    tile = this->getTile(this);
    this->drawBackground(this, tile?0:refresh);
    if (!tile) return;

    s = getScreen();
    sf = SDL_GetVideoSurface();
    s->coordinatesToRect(s, this->x, this->y, 1, 1, &drawArea);
    SDL_BlitSurface((SDL_Surface *)tile, 0, sf, &drawArea);

    if (refresh)
    {
	SDL_UpdateRect(sf, drawArea.x, drawArea.y, drawArea.w, drawArea.h);
    }
}

static void
m_drawBackground(THIS, int refresh)
{
    METHOD(Entity);

    SDL_Rect drawArea;
    const SDL_Surface *tile;
    Screen s;
    SDL_Surface *sf;
    unsigned int n;

    s = getScreen();
    s->coordinatesToRect(s, this->x, this->y, 1, 1, &drawArea);
    sf = SDL_GetVideoSurface();

    switch (this->bg)
    {
	case BG_None:
	    break;

	case BG_Empty:
	    n = calculateNeighbors(this);
	    tile = s->getTile(s, emptyTileTable[n&15][0], emptyTileTable[n&15][1]);
	    SDL_BlitSurface((SDL_Surface *)tile, 0, sf, &drawArea);	    
	    if (n&128)
	    {
		tile = s->getTile(s, SATN_Corner, 0);
		SDL_BlitSurface((SDL_Surface *)tile, 0, sf, &drawArea);
	    }
	    if (n&64)
	    {
		tile = s->getTile(s, SATN_Corner, 1);
		SDL_BlitSurface((SDL_Surface *)tile, 0, sf, &drawArea);
	    }
	    if (n&32)
	    {
		tile = s->getTile(s, SATN_Corner, 2);
		SDL_BlitSurface((SDL_Surface *)tile, 0, sf, &drawArea);
	    }
	    if (n&16)
	    {
		tile = s->getTile(s, SATN_Corner, 3);
		SDL_BlitSurface((SDL_Surface *)tile, 0, sf, &drawArea);
	    }
	    break;

	case BG_Earth:
	    tile = s->getTile(s, SATN_Earth, 0);
	    SDL_BlitSurface((SDL_Surface *)tile, 0, sf, &drawArea);
	    break;
    }

    if (refresh)
    {
	SDL_UpdateRect(sf, drawArea.x, drawArea.y, drawArea.w, drawArea.h);
    }
}

static void
m_dispose(THIS)
{
    METHOD(Entity);
    DestroyEvent(this->PositionChanged);
    DestroyEvent(this->MoveStarting);
    DELETE(Entity, this);
}

CTOR(Entity)
{
    BASECTOR(Entity, Object);
    this->init = &m_init;
    this->dispose = &m_dispose;
    this->getTile = &m_getTile;
    this->draw = &m_draw;
    this->drawBackground = &m_drawBackground;
    this->bg = BG_Empty;
    return this;
}

DTOR(Entity)
{
    BASEDTOR(Object);
}

