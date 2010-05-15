#include <SDL_rotozoom.h>
#include <SDL_image.h>
#include <stdlib.h>

#include "board.h"
#include "resfile.h"
#include "resource.h"
#include "app.h"
#include "event.h"

#include "eearth.h"
#include "ewall.h"
#include "erock.h"
#include "ecabbage.h"
#include "ewilly.h"

static SDL_Rect drawArea[3];

struct MoveRecord;
typedef struct MoveRecord *MoveRecord;

struct MoveRecord
{
    Entity e;
    int dir_x;
    int dir_y;
    int off_x;
    int off_y;
    MoveRecord prev;
    MoveRecord next;
};

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

    MoveRecord move;

    SDL_TimerID moveticker;

    int tile_width;
    int tile_height;

    int step_x;
    int step_y;

    int willy_x;
    int willy_y;

    Entity entity[32][24];
    int num_rocks;
    ERock rock[32*24];
};

static void checkRocks(Board b);

Uint32
createMoveTickEvent(Uint32 interval, void *param)
{
    Board b = CAST(param, Board);

    Event e = NEW(Event);
    e->sender = b;
    e->handler = b;
    e->type = SAEV_MoveTick;

    RaiseEvent(e);
    
    return interval;
}

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
internal_put(Board b, int x, int y, Entity e)
{
    ERock rock = CAST(e, ERock);
    if (rock) b->pimpl->rock[b->pimpl->num_rocks++] = rock;
    b->pimpl->entity[x][y] = e;
}

static void
internal_draw(Board b, int x, int y, MoveRecord m, int refresh)
{
    SDL_Surface *screen;
    const SDL_Surface *tile;
    const SDL_Surface *base;
    Entity e;
    Event ev;

    if (m) e = m->e;
    else e = b->pimpl->entity[x][y];

    drawArea[0].x = x * drawArea[0].w;
    drawArea[0].y = y * drawArea[0].h;

    screen = b->pimpl->screen;

    base = 0;

    if (e)
    {
	if (e->getBaseSurface)
	{
	    base = e->getBaseSurface(b);
	}
	tile = e->getSurface(b);
    }
    else
    {
	tile = b->getEmptyTile(b);
    }

    SDL_BlitSurface((SDL_Surface *)base, 0, screen, &drawArea[0]);
    if (m)
    {
	drawArea[1].x = drawArea[0].x + m->dir_x * base->w;
	drawArea[1].y = drawArea[0].y + m->dir_y * base->h;
	SDL_BlitSurface((SDL_Surface *)base, 0, screen, &drawArea[1]);
	if (m->dir_x) m->off_x += b->pimpl->step_x;
	if (m->dir_y) m->off_y += b->pimpl->step_y;
	if ((base->w - m->off_x < b->pimpl->step_x)
		|| (base->h - m->off_y < b->pimpl->step_y))
	{
	    drawArea[2].x = drawArea[1].x;
	    drawArea[2].y = drawArea[1].y;
	    if (m->prev)
	    {
		if (m->next)
		    m->next->prev = m->prev;
		m->prev->next = m->next;
	    }
	    else
	    {
		b->pimpl->move = 0;
	    }
	    b->pimpl->entity[m->e->x][m->e->y] = 0;
	    m->e->x += m->dir_x;
	    m->e->y += m->dir_y;
	    b->pimpl->entity[m->e->x][m->e->y] = m->e;

	    Event ev = NEW(Event);
	    ev->sender = b;
	    ev->handler = m->e;
	    ev->type = SAEV_MoveFinished;
	    RaiseEvent(ev);

	    XFREE(m);

	    checkRocks(b);
	}
	else
	{
	    drawArea[2].x = drawArea[0].x + m->dir_x * m->off_x;
	    drawArea[2].y = drawArea[0].y + m->dir_y * m->off_y;
	}
	SDL_BlitSurface((SDL_Surface *)tile, 0, screen, &drawArea[2]);
	if (refresh) SDL_UpdateRects(screen, 2, &drawArea[0]);
    }
    else
    {
	SDL_BlitSurface((SDL_Surface *)tile, 0, screen, &drawArea[0]);
	if (refresh) SDL_UpdateRects(screen, 1, &drawArea[0]);
    }
}

static void
m_handleEvent ARG(Event e)
{
    METHOD(Board);

    MoveRecord m;
    MoveRecord next;

    if (e->type == SAEV_MoveTick)
    {
	if (!this->pimpl->move)
	{
	    SDL_RemoveTimer(this->pimpl->moveticker);
	    this->pimpl->moveticker = 0;
	}
	else
	{
	    m = this->pimpl->move;
	    while (m)
	    {
		next = m->next;
		internal_draw(this, m->e->x, m->e->y, m, 1);
		m = next;
	    }
	}
    }

    DELETE(Event, e);
}

static void
m_draw ARG(int x, int y, int refresh)
{
    METHOD(Board);
    internal_draw(this, x, y, 0, refresh);
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

static int
m_isEmpty ARG(int x, int y)
{
    METHOD(Board);

    if ((x<0)||(x>32)||(y<0)||(y>24)) return 0;
    return (this->pimpl->entity[x][y] ? 0 : 1);
}

static void
m_startMove ARG(Entity e, int dir_x, int dir_y)
{
    METHOD(Board);

    MoveRecord last;

    MoveRecord m = XMALLOC(struct MoveRecord, 1);
    m->dir_x = dir_x;
    m->dir_y = dir_y;
    m->off_x = 0;
    m->off_y = 0;
    m->e = e;
    m->prev = 0;
    m->next = 0;

    if (!this->pimpl->move)
    {
	this->pimpl->move = m;
    }
    else
    {
	last = this->pimpl->move;
	while (last->next) last = last->next;
	m->prev = last;
	last->next = m;
    }
    if (!this->pimpl->moveticker)
    {
	this->pimpl->moveticker = SDL_AddTimer(
		30, &createMoveTickEvent, this);
    }
}

static void
checkRocks(Board b)
{
    int i;
    ERock r;
    Entity e;

    for (i=0; i<b->pimpl->num_rocks; ++i)
    {
	r = b->pimpl->rock[i];
	e = CAST(r, Entity);
	if (m_isEmpty(b, e->x, e->y + 1)) r->fall(r);
    }
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

static void
m_initVideo ARG()
{
    METHOD(Board);

    struct Board_impl *b = this->pimpl;

    b->screen = SDL_GetVideoSurface();
    b->tile_width = b->screen->w / 32;
    b->tile_height = b->screen->h / 24;

    drawArea[0].w = drawArea[1].w = drawArea[2].w = b->tile_width;
    drawArea[0].h = drawArea[1].h = drawArea[2].h = b->tile_height;

    if (b->tile_width < 4) b->step_x = 1;
    else b->step_x = b->tile_width / 4;

    if (b->tile_height < 4) b->step_y = 1;
    else b->step_y = b->tile_height / 4;

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
    checkRocks(this);
}

CTOR(Board)
{
    BASECTOR(Board, EHandler);

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

    ((EHandler)this)->handleEvent = &m_handleEvent;
    this->initVideo = &m_initVideo;
    this->redraw = &m_redraw;
    this->draw = &m_draw;
    this->isEmpty = &m_isEmpty;
    this->startMove = &m_startMove;
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
	    if(e)
	    {
		e->init(e, this, x, y);
		internal_put(this, x, y, e);
	    }
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
    BASEDTOR(EHandler);
}

