#include <SDL_rotozoom.h>

#include "board.h"
#include "resfile.h"
#include "app.h"

#include "eempty.h"

Resfile rf;

struct Board_impl
{
    SDL_Surface *screen;
    SDL_Surface *tile_empty;
    SDL_Surface *tile_earth;
    SDL_Surface *tile_wall;
    SDL_Surface *tile_rock;
    SDL_Surface *tile_cabbage;
    SDL_Surface *tile_willy;

    int width;
    int height;
    int tile_width;
    int tile_height;

    Entity entity[32][24];
};

SDL_Surface *
createScaledSurface(Resource r, int width, int height)
{
    SDL_Surface *tile;
    SDL_Surface *tmp;

    SDL_RWops *rw = SDL_RWFromMem(r->getData(r), r->getDataSize(r));
    tile = SDL_LoadBMP_RW(rw, 1);
    tmp = zoomSurface (tile, (double)width/64, (double)height/64, SMOOTHING_ON);
    SDL_FreeSurface(tile);
    tile = SDL_DisplayFormat(tmp);
    SDL_FreeSurface(tmp);
    return tile;
}


static void
m_init ARG(SDL_Surface *screen, int width, int height)
{
    METHOD(Board);

    Resource r;

    this->pimpl->screen = screen;
    this->pimpl->width = width;
    this->pimpl->height = height;
    this->pimpl->tile_width = width / 32;
    this->pimpl->tile_height = height / 24;

    rf = NEW(Resfile);
    rf->setFile(rf, "res/tiles.res");
    if (rf->open(rf, 0) < 0)
    {
	log_err("Error loading tiles.\n");
	mainApp->abort(mainApp);
    }

    rf->load(rf, "tile_empty", &r);
    this->pimpl->tile_empty = createScaledSurface(r,
	    this->pimpl->tile_width, this->pimpl->tile_height);
    DELETE(Resource, r);
    rf->load(rf, "tile_earth", &r);
    this->pimpl->tile_earth = createScaledSurface(r,
	    this->pimpl->tile_width, this->pimpl->tile_height);
    DELETE(Resource, r);
    rf->load(rf, "tile_wall", &r);
    this->pimpl->tile_wall = createScaledSurface(r,
	    this->pimpl->tile_width, this->pimpl->tile_height);
    DELETE(Resource, r);
    rf->load(rf, "tile_rock", &r);
    this->pimpl->tile_rock = createScaledSurface(r,
	    this->pimpl->tile_width, this->pimpl->tile_height);
    DELETE(Resource, r);
    rf->load(rf, "tile_cabbage", &r);
    this->pimpl->tile_cabbage = createScaledSurface(r,
	    this->pimpl->tile_width, this->pimpl->tile_height);
    DELETE(Resource, r);
    rf->load(rf, "tile_willy", &r);
    this->pimpl->tile_willy = createScaledSurface(r,
	    this->pimpl->tile_width, this->pimpl->tile_height);
    DELETE(Resource, r);

    int x, y;
    Entity e;
    for (x = 0; x < 32; ++x)
	for (y = 0; y < 24; ++y)
	{
	    e = (Entity)NEW(EEmpty);
	    e->init(e, this, x, y);
	    this->pimpl->entity[x][y] = e;
	    e->draw(e);
	}

}

static const SDL_Surface *
m_getEmptyTile ARG()
{
    METHOD(Board);

    return this->pimpl->tile_empty;
}

static const SDL_Surface *
m_getEarthTile ARG()
{
    METHOD(Board);

    return this->pimpl->tile_earth;
}

static const SDL_Surface *
m_getWallTile ARG()
{
    METHOD(Board);

    return this->pimpl->tile_wall;
}

static const SDL_Surface *
m_getRockTile ARG()
{
    METHOD(Board);

    return this->pimpl->tile_rock;
}

static const SDL_Surface *
m_getCabbageTile ARG()
{
    METHOD(Board);

    return this->pimpl->tile_cabbage;
}

static const SDL_Surface *
m_getWillyTile ARG()
{
    METHOD(Board);

    return this->pimpl->tile_willy;
}

static SDL_Surface *
m_getScreen ARG()
{
    METHOD(Board);

    return this->pimpl->screen;
}

CTOR(Board)
{
    BASECTOR(Board, Object);
    this->pimpl = XMALLOC(struct Board_impl, 1);
    memset(this->pimpl, 0, sizeof(struct Board_impl));
    this->init = &m_init;
    this->getEmptyTile = &m_getEmptyTile;
    this->getEarthTile = &m_getEarthTile;
    this->getWallTile = &m_getWallTile;
    this->getRockTile = &m_getRockTile;
    this->getCabbageTile = &m_getCabbageTile;
    this->getWillyTile = &m_getWillyTile;
    this->getScreen = &m_getScreen;
    return this;
}

DTOR(Board)
{
    int i;
    Entity e;
    Entity *ep;

    if (this->pimpl->tile_empty) SDL_FreeSurface(this->pimpl->tile_empty);
    if (this->pimpl->tile_earth) SDL_FreeSurface(this->pimpl->tile_earth);
    if (this->pimpl->tile_wall) SDL_FreeSurface(this->pimpl->tile_wall);
    if (this->pimpl->tile_rock) SDL_FreeSurface(this->pimpl->tile_rock);
    if (this->pimpl->tile_cabbage) SDL_FreeSurface(this->pimpl->tile_cabbage);
    if (this->pimpl->tile_willy) SDL_FreeSurface(this->pimpl->tile_willy);

    ep = (Entity *)&(this->pimpl->entity);
    for (i=0; i<32*24; ++i)
    {
	e = ep[i];
	if(e) e->dispose(e);
    }

    XFREE(this->pimpl);
    BASEDTOR(Object);
}

