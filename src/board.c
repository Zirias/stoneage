#include <SDL_rotozoom.h>
#include <SDL_image.h>
#include <stdlib.h>

#include "board.h"
#include "resfile.h"
#include "app.h"

#include "eearth.h"
#include "ewall.h"
#include "erock.h"
#include "ecabbage.h"
#include "ewilly.h"

static SDL_Rect drawArea;

struct Board_impl
{
    SDL_Surface *screen;

    Resource t_empty;
    Resource t_earth;
    Resource t_wall;
    Resource t_rock;
    Resource t_cabbage;
    Resource t_willy;

    SDL_Surface *s_empty;
    SDL_Surface *s_earth;
    SDL_Surface *s_wall;
    SDL_Surface *s_rock;
    SDL_Surface *s_cabbage;
    SDL_Surface *s_willy;

    int tile_width;
    int tile_height;

    int willy_x;
    int willy_y;

    Entity entity[32][24];
};

SDL_Surface *
createScaledSurface(Resource r, int width, int height)
{
    SDL_Surface *tile;
    SDL_Surface *tmp;

    SDL_RWops *rw = SDL_RWFromMem((void *)r->getData(r), r->getDataSize(r));
    tmp = IMG_LoadPNG_RW(rw);
    tile = zoomSurface (tmp, (double)width/64, (double)height/64, SMOOTHING_ON);
    SDL_FreeSurface(tmp);
    SDL_SetAlpha(tile, SDL_SRCALPHA|SDL_RLEACCEL, SDL_ALPHA_OPAQUE);
    return tile;
}

static void
m_draw ARG(int x, int y, int refresh)
{
    METHOD(Board);

    SDL_Surface *screen;
    const SDL_Surface *tile;
    Entity e;

    e = this->pimpl->entity[x][y];
    tile = this->getEmptyTile(this);

    drawArea.w = tile->w;
    drawArea.h = tile->h;
    drawArea.x = x * drawArea.w;
    drawArea.y = y * drawArea.h;

    screen = this->pimpl->screen;

    if (e)
    {
	if (e->getBaseSurface)
	{
	    tile = e->getBaseSurface(this);
	    SDL_BlitSurface((SDL_Surface *)tile, NULL, screen, &drawArea);
	}
	tile = e->getSurface(this);
    }
    SDL_BlitSurface((SDL_Surface *)tile, NULL, screen, &drawArea);

    if (refresh) SDL_UpdateRects(screen, 1, &drawArea);
}

static void
m_redraw ARG()
{
    METHOD(Board);
    Entity e;
    int x, y;

    struct Board_impl *b = this->pimpl;

    for (x = 0; x < 32; ++x)
	for (y = 0; y < 24; ++y)
	    this->draw(this, x, y, 0);

    SDL_UpdateRect(b->screen, 0, 0, b->screen->w, b->screen->h);
}

static void
m_initVideo ARG()
{
    METHOD(Board);

    struct Board_impl *b = this->pimpl;

    b->screen = SDL_GetVideoSurface();
    b->tile_width = b->screen->w / 32;
    b->tile_height = b->screen->h / 24;

    if (b->s_empty) SDL_FreeSurface(b->s_empty);
    if (b->s_earth) SDL_FreeSurface(b->s_earth);
    if (b->s_wall) SDL_FreeSurface(b->s_wall);
    if (b->s_rock) SDL_FreeSurface(b->s_rock);
    if (b->s_cabbage) SDL_FreeSurface(b->s_cabbage);
    if (b->s_willy) SDL_FreeSurface(b->s_willy);

    b->s_empty = createScaledSurface(
	    b->t_empty, b->tile_width, b->tile_height);
    b->s_earth = createScaledSurface(
	    b->t_earth, b->tile_width, b->tile_height);
    b->s_wall = createScaledSurface(
	    b->t_wall, b->tile_width, b->tile_height);
    b->s_rock = createScaledSurface(
	    b->t_rock, b->tile_width, b->tile_height);
    b->s_cabbage = createScaledSurface(
	    b->t_cabbage, b->tile_width, b->tile_height);
    b->s_willy = createScaledSurface(
	    b->t_willy, b->tile_width, b->tile_height);

    this->redraw(this);
}

static const SDL_Surface *
m_getEmptyTile ARG()
{
    METHOD(Board);

    return this->pimpl->s_empty;
}

static const SDL_Surface *
m_getEarthTile ARG()
{
    METHOD(Board);

    return this->pimpl->s_earth;
}

static const SDL_Surface *
m_getWallTile ARG()
{
    METHOD(Board);

    return this->pimpl->s_wall;
}

static const SDL_Surface *
m_getRockTile ARG()
{
    METHOD(Board);

    return this->pimpl->s_rock;
}

static const SDL_Surface *
m_getCabbageTile ARG()
{
    METHOD(Board);

    return this->pimpl->s_cabbage;
}

static const SDL_Surface *
m_getWillyTile ARG()
{
    METHOD(Board);

    return this->pimpl->s_willy;
}

CTOR(Board)
{
    BASECTOR(Board, Object);

    struct Board_impl *b;
    Resfile rf;

    int imgflags = IMG_Init(IMG_INIT_PNG);
    if (!(imgflags & IMG_INIT_PNG))
    {
	log_err("SDL_image library is missing PNG support!\n"
		"Try installing libpng and zlib.\n");
	mainApp->abort(mainApp);
    }

    b = XMALLOC(struct Board_impl, 1);
    memset(b, 0, sizeof(struct Board_impl));
    this->pimpl = b;

    this->initVideo = &m_initVideo;
    this->redraw = &m_redraw;
    this->draw = &m_draw;
    this->getEmptyTile = &m_getEmptyTile;
    this->getEarthTile = &m_getEarthTile;
    this->getWallTile = &m_getWallTile;
    this->getRockTile = &m_getRockTile;
    this->getCabbageTile = &m_getCabbageTile;
    this->getWillyTile = &m_getWillyTile;

    rf = NEW(Resfile);
    rf->setFile(rf, RES_GFX);
    if (rf->open(rf, 0) < 0)
    {
	log_err("Error loading tiles.\n");
	mainApp->abort(mainApp);
    }

    rf->load(rf, "tile_empty", &(b->t_empty));
    rf->load(rf, "tile_earth", &(b->t_earth));
    rf->load(rf, "tile_wall", &(b->t_wall));
    rf->load(rf, "tile_rock", &(b->t_rock));
    rf->load(rf, "tile_cabbage", &(b->t_cabbage));
    rf->load(rf, "tile_willy", &(b->t_willy));
    if (!b->t_empty || !b->t_earth || !b->t_wall || !b->t_rock
	    || !b->t_cabbage || !b->t_willy)
    {
	log_err("Error loading tiles.\n");
	mainApp->abort(mainApp);
    }

    rf->close(rf);
    DELETE(Resfile, rf);

    b->willy_x = -1;
    b->willy_y = -1;

    int x, y, z;
    Entity e;
    for (x = 0; x < 32; ++x)
	for (y = 0; y < 24; ++y)
	{
	    z = (random() / (RAND_MAX / 5));
	    switch(z)
	    {
		case 0 : e = 0; break;
		case 1 : e = (Entity)NEW(EEarth); break;
		case 2 : e = (Entity)NEW(EWall); break;
		case 3 : e = (Entity)NEW(ERock); break;
		case 4 : e = (Entity)NEW(ECabbage); break;
	    }
	    if(e) e->init(e, this, x, y);
	    b->entity[x][y] = e;
	}
    b->willy_x = x = (random() / (RAND_MAX / 32));
    b->willy_y = y = (random() / (RAND_MAX / 24));
    e = (Entity)NEW(EWilly);
    e->init(e, this, x, y);
    DELETE(Entity, b->entity[x][y]);
    b->entity[x][y] = e;

    this->initVideo(this);
    return this;
}

DTOR(Board)
{
    int i;
    Entity e;
    Entity *ep;

    struct Board_impl *b = this->pimpl;

    if (b->s_empty) SDL_FreeSurface(b->s_empty);
    if (b->s_earth) SDL_FreeSurface(b->s_earth);
    if (b->s_wall) SDL_FreeSurface(b->s_wall);
    if (b->s_rock) SDL_FreeSurface(b->s_rock);
    if (b->s_cabbage) SDL_FreeSurface(b->s_cabbage);
    if (b->s_willy) SDL_FreeSurface(b->s_willy);

    DELETE(Resource, b->t_empty);
    DELETE(Resource, b->t_earth);
    DELETE(Resource, b->t_wall);
    DELETE(Resource, b->t_rock);
    DELETE(Resource, b->t_cabbage);
    DELETE(Resource, b->t_willy);

    ep = (Entity *)&(b->entity);
    for (i=0; i<32*24; ++i)
    {
	e = ep[i];
	if(e) e->dispose(e);
    }

    IMG_Quit();
    XFREE(this->pimpl);
    BASEDTOR(Object);
}

