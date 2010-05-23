#include <SDL_rotozoom.h>
#include <SDL_image.h>
#include <stdlib.h>

#include "board.h"
#include "resfile.h"
#include "resource.h"
#include "app.h"
#include "event.h"
#include "level.h"
#include "move.h"

#include "eearth.h"
#include "ewall.h"
#include "erock.h"
#include "ecabbage.h"
#include "ewilly.h"

static SDL_Rect drawArea;

struct BlitSequence;
typedef struct BlitSequence *BlitSequence;

struct BlitSequence
{
    int n;
    SDL_Surface *blits[16];
};

struct Board_impl
{
    SDL_Surface *screen;

    Resource t_empty;
    Resource t_empty_1;
    Resource t_empty_2a;
    Resource t_empty_2f;
    Resource t_empty_3;
    Resource t_empty_4;
    Resource t_corner;
    Resource t_earth;
    Resource t_wall;
    Resource t_rock;
    Resource t_cabbage;
    Resource t_willy;

    SDL_Surface *s_empty[16];
    SDL_Surface *s_corner[4];
    SDL_Surface *s_earth;
    SDL_Surface *s_wall;
    SDL_Surface *s_rock;
    SDL_Surface *s_cabbage;
    SDL_Surface *s_willy;

    Move movelist;

    SDL_TimerID moveticker;

    int tile_width;
    int tile_height;

    Entity entity[LVL_ROWS][LVL_COLS];
    int num_rocks;
    ERock rock[LVL_ROWS * LVL_COLS];
};

static void checkRocks(Board b);

Uint32
createMoveTickEvent(Uint32 interval, void *param)
{
    Board b = CAST(param, Board);

    Event e = NEW(Event);
    e->sender = CAST(b, Object);
    e->handler = CAST(b, EHandler);
    e->type = SAEV_MoveTick;

    RaiseEvent(e);
    
    return interval;
}

SDL_Surface *
loadPngSurface(Resource r)
{
    SDL_Surface *png;
    SDL_RWops *rw = SDL_RWFromMem((void *)r->getData(r), r->getDataSize(r));
    png = IMG_LoadPNG_RW(rw);
    SDL_FreeRW(rw);
    return png;
}

SDL_Surface *
rotateSurface(SDL_Surface *s, int rot)
{
    int x,y;
    uint32_t *srcpix, *dstpix;

    SDL_Surface *dest = SDL_CreateRGBSurface(SDL_SWSURFACE,
	    (rot % 2) ? s->w : s->h,
	    (rot % 2) ? s->h : s->w, 
	    32,
	    s->format->Rmask,
	    s->format->Gmask,
	    s->format->Bmask,
	    s->format->Amask);
    SDL_LockSurface(s);
    SDL_LockSurface(dest);
    srcpix = (uint32_t *)s->pixels;
    dstpix = (uint32_t *)dest->pixels;
    if (rot == 1)
    {
	for (x=dest->w-1;x>=0;--x) for (y=0;y<dest->h;++y)
	    dstpix[y*dest->w+x] = *srcpix++;
    }
    else if (rot == 2)
    {
	for (y=dest->h-1;y>=0;--y) for (x=dest->w-1;x>=0;--x)
	    dstpix[y*dest->w+x] = *srcpix++;
    }
    else
    {
	for (x=0;x<dest->w;++x) for (y=dest->h-1;y>=0;--y)
	    dstpix[y*dest->w+x] = *srcpix++;
    }
    SDL_UnlockSurface(s);
    SDL_UnlockSurface(dest);
    return dest;
}

SDL_Surface *
scaleSurface(SDL_Surface *s, int width, int height, int rot)
{
    SDL_Surface *tile;
    SDL_Surface *tmp;

    if (rot)
    {
	tile = rotateSurface(s, rot);
	tmp = zoomSurface (tile, (double)width/64,
		(double)height/64, SMOOTHING_ON);
	SDL_FreeSurface(tile);
    }
    else
    {
	tmp = zoomSurface (s, (double)width/64,
		(double)height/64, SMOOTHING_ON);
    }
    tile = SDL_DisplayFormatAlpha(tmp);
    SDL_FreeSurface(tmp);
    return tile;
}

SDL_Surface *
createScaledSurface(Resource r, int width, int height)
{
    SDL_Surface *png;
    SDL_Surface *tile;

    png = loadPngSurface(r);
    tile = scaleSurface(png, width, height, 0);
    SDL_FreeSurface(png);
    return tile;
}

static void
findRocks(Board b)
{
    int x, y;
    Entity e;
    ERock r;

    b->pimpl->num_rocks = 0;
    for (y=0; y<LVL_ROWS; ++y) for (x=0; x<LVL_COLS; ++x)
    {
	e = b->pimpl->entity[y][x];
	if (!e) continue;
	r = CAST(e, ERock);
	if (r) b->pimpl->rock[b->pimpl->num_rocks++] = r;
    }
}

static void
moveStep(Board b, Move m)
{
    int done;
    int tw, th;
    SDL_Rect dirty;
    Entity e;
    Event ev;

    e = m->entity(m);
    tw = b->pimpl->tile_width;
    th = b->pimpl->tile_height;
    dirty.x = e->x * tw;
    dirty.y = e->y * th;
    dirty.w = tw;
    dirty.h = th;

    b->draw(b, e->x, e->y, 0);
    if (m->dx)
    {
	b->draw(b, e->x + m->dx, e->y, 0);
	if (m->dx < 0) dirty.x -= tw;
	dirty.w += tw;
    }
    if (m->dy)
    {
	b->draw(b, e->x, e->y + m->dy, 0);
	if (m->dy < 0) dirty.y -= th;
	dirty.h += th;
    }
    if (m->dx && m->dy) b->draw(b, e->x + m->dx, e->y + m->dy, 0);

    done = m->step(m, &drawArea.x, &drawArea.y);
    SDL_BlitSurface((SDL_Surface *)e->getSurface(b),
	    0, b->pimpl->screen, &drawArea);
    SDL_UpdateRects(b->pimpl->screen, 1, &dirty);

    if (done)
    {
	if (m->prev)
	{
	    m->prev->next = m->next;
	}
	else
	{
	    b->pimpl->movelist = m->next;
	}
	if (m->next)
	{
	    m->next->prev = m->prev;
	}
	b->pimpl->entity[e->y][e->x] = 0;
	e->x += m->dx;
	e->y += m->dy;
	e->m = 0;
	b->pimpl->entity[e->y][e->x] = e;

	ev = NEW(Event);
	ev->sender = CAST(b, Object);
	ev->handler = CAST(e, EHandler);
	ev->type = SAEV_MoveFinished;
	RaiseEvent(ev);

	DELETE(Move, m);

	checkRocks(b);

	if (!b->pimpl->movelist)
	{
	    /* all moves finished, unfreeze willy */
	    getWilly()->moveLock = 0;
	}
    }
}

static void
randomLevel(Board b)
{
    Level l = NEW(Level);
    l->random(l);
    l->createEntities(l, b, (Entity *)&b->pimpl->entity);
    DELETE(Level, l);
}

static void
internal_clear(Board b)
{
    Move m, next;
    Entity *e;
    Entity *end = (Entity *)&(b->pimpl->entity) + LVL_COLS*LVL_ROWS;
    int i;

    if (b->pimpl->moveticker)
    {
	SDL_RemoveTimer(b->pimpl->moveticker);
	b->pimpl->moveticker = 0;
	CancelEventsFor(CAST(b, EHandler));
	m = b->pimpl->movelist;
	b->pimpl->movelist = 0;
	while (m)
	{
	    next = m->next;
	    DELETE(Move, m);
	    m = next;
	}
    }

    for (e = (Entity *) &(b->pimpl->entity); e != end; ++e)
    {
	if(*e)
	{
	    CancelEventsFor(CAST(*e, EHandler));
	    (*e)->dispose(*e);
	}
	*e = 0;
    }
    
    b->pimpl->num_rocks = 0;
}

static void
m_handleEvent(THIS, Event e)
{
    METHOD(Board);

    Move m;
    Move next;

    if (e->type == SAEV_MoveTick)
    {
	m = this->pimpl->movelist;
	if (!m)
	{
	    SDL_RemoveTimer(this->pimpl->moveticker);
	    this->pimpl->moveticker = 0;
	}
	else
	{
	    while (m)
	    {
		next = m->next;
		moveStep(this, m);
		m = next;
	    }
	}
    }
    
    DELETE(Event, e);
}

static void
m_draw(THIS, int x, int y, int refresh)
{
    METHOD(Board);

    SDL_Surface *screen;
    const SDL_Surface *tile;
    struct BlitSequence base;
    int drawBase;
    Entity e;
    int i;

    if ((x<0)||(x>LVL_COLS-1)||(y<0)||(y>LVL_ROWS-1)) return;

    e = this->pimpl->entity[y][x];

    drawArea.x = x * drawArea.w;
    drawArea.y = y * drawArea.h;

    screen = this->pimpl->screen;

    drawBase = 0;
    tile = 0;

    if (e)
    {
	if (e->getBaseSurface)
	{
	    e->getBaseSurface(this, x, y, &base);
	    drawBase = 1;
	}
	tile = e->getSurface(this);
    }
    else
    {
	this->getEmptyTile(this, x, y, &base);
	drawBase = 1;
    }

    if (drawBase) for (i=0; i<base.n; ++i)
	SDL_BlitSurface(base.blits[i], 0, screen, &drawArea);


    if (tile && !e->m)
	SDL_BlitSurface((SDL_Surface *)tile, 0, screen, &drawArea);

    if (refresh)
	SDL_UpdateRects(screen, 1, &drawArea);
}

static void
m_redraw(THIS)
{
    METHOD(Board);
    Entity e;
    int x, y;

    struct Board_impl *b = this->pimpl;

    for (y = 0; y < LVL_ROWS; ++y)
	for (x = 0; x < LVL_COLS; ++x)
	    this->draw(this, x, y, 0);

    SDL_UpdateRect(b->screen, 0, 0, b->screen->w, b->screen->h);
}

static int
m_coordinatesToPixel(THIS, int x, int y, int *px, int *py)
{
    METHOD(Board);

    if ((x<0)||(x>LVL_COLS-1)||(y<0)||(y>LVL_ROWS-1)) return -1;
    *px = this->pimpl->tile_width * x;
    *py = this->pimpl->tile_height * y;
    return 0;
}

static int
m_entity(THIS, int x, int y, Entity *e)
{
    METHOD(Board);

    if ((x<0)||(x>LVL_COLS-1)||(y<0)||(y>LVL_ROWS-1)) return -1;
    *e = this->pimpl->entity[y][x];
    return 0;
}

static void
m_startMove(THIS, Move m)
{
    METHOD(Board);

    if (this->pimpl->movelist) 
    {
	this->pimpl->movelist->prev = m;
	m->next = this->pimpl->movelist;
    }
    this->pimpl->movelist = m;

    if (!this->pimpl->moveticker)
    {
	this->pimpl->moveticker = SDL_AddTimer(
		20, &createMoveTickEvent, this);
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
	if (e->y<LVL_ROWS-1 && !b->pimpl->entity[e->y+1][e->x])
	{
	    /* freeze willy and let rock fall */
	    getWilly()->moveLock = 1;
	    r->fall(r);
	}
    }
}

static int
isSolidTile(Board this, int x, int y)
{
    Entity e = this->pimpl->entity[y][x];
    if (!e) return 0;
    if (CAST(e, ECabbage)) return 1;
    if (CAST(e, EEarth)) return 1;
    return 0;
}

static unsigned int
calculateNeighbors(Board this, int x, int y)
{
    struct Board_impl *b = this->pimpl;

    unsigned int neighbors = 0;
    if ((y > 0) && isSolidTile(this, x, y-1)) neighbors += 8;
    if ((x < LVL_COLS-1) && isSolidTile(this, x+1, y)) neighbors += 4;
    if ((y < LVL_ROWS-1) && isSolidTile(this, x, y+1)) neighbors += 2;
    if ((x > 0) && isSolidTile(this, x-1, y)) neighbors += 1;
    return neighbors;
}

static void
m_getEmptyTile(THIS, int x, int y, void *buf)
{
    METHOD(Board);
    
    BlitSequence bs = (BlitSequence)buf;
    unsigned int n = calculateNeighbors(this, x, y);
    bs->n = 0;
    bs->blits[bs->n++] = this->pimpl->s_empty[n];
    if (!(n&(8|4)) && (y>0) && (x<LVL_COLS-1)
	    && isSolidTile(this, x+1, y-1))
	bs->blits[bs->n++] = this->pimpl->s_corner[0];
    if (!(n&(4|2)) && (y<LVL_ROWS-1) && (x<LVL_COLS-1)
	    && isSolidTile(this, x+1, y+1))
	bs->blits[bs->n++] = this->pimpl->s_corner[1];
    if (!(n&(2|1)) && (y<LVL_ROWS-1) && (x>0)
	    && isSolidTile(this, x-1, y+1))
	bs->blits[bs->n++] = this->pimpl->s_corner[2];
    if (!(n&(8|1)) && (y>0) && (x>0)
	    && isSolidTile(this, x-1, y-1))
	bs->blits[bs->n++] = this->pimpl->s_corner[3];
}

static void
m_getEarthBaseTile(THIS, int x, int y, void *buf)
{
    METHOD(Board);

    BlitSequence bs = (BlitSequence)buf;
    bs->n = 1;
    bs->blits[0] = this->pimpl->s_earth;
}

static const SDL_Surface *
m_getEarthTile(THIS)
{
    METHOD(Board);

    return this->pimpl->s_earth;
}

static const SDL_Surface *
m_getWallTile(THIS)
{
    METHOD(Board);

    return this->pimpl->s_wall;
}

static const SDL_Surface *
m_getRockTile(THIS)
{
    METHOD(Board);

    return this->pimpl->s_rock;
}

static const SDL_Surface *
m_getCabbageTile(THIS)
{
    METHOD(Board);

    return this->pimpl->s_cabbage;
}

static const SDL_Surface *
m_getWillyTile(THIS)
{
    METHOD(Board);

    return this->pimpl->s_willy;
}

static void
m_loadLevel(THIS)
{
    METHOD(Board);

    internal_clear(this);
    randomLevel(this);
    findRocks(this);
    this->redraw(this);
    checkRocks(this);
}

static void
freeSdlSurfaces(Board this)
{
    struct Board_impl *b = this->pimpl;
    SDL_Surface **p;
    SDL_Surface **p_end;

    p_end = (SDL_Surface **)b->s_empty + 16;
    for (p = (SDL_Surface **)b->s_empty; p != p_end; ++p)
	if (*p) SDL_FreeSurface(*p);
    p_end = (SDL_Surface **)b->s_corner + 4;
    for (p = (SDL_Surface **)b->s_corner; p != p_end; ++p)
	if (*p) SDL_FreeSurface(*p);
    if (b->s_earth) SDL_FreeSurface(b->s_earth);
    if (b->s_wall) SDL_FreeSurface(b->s_wall);
    if (b->s_rock) SDL_FreeSurface(b->s_rock);
    if (b->s_cabbage) SDL_FreeSurface(b->s_cabbage);
    if (b->s_willy) SDL_FreeSurface(b->s_willy);
}

static void
m_initVideo(THIS)
{
    METHOD(Board);

    SDL_Surface *png;
    int i;

    struct Board_impl *b = this->pimpl;

    b->screen = SDL_GetVideoSurface();
    b->tile_width = b->screen->w / LVL_COLS;
    b->tile_height = b->screen->h / LVL_ROWS;
    computeTrajectories(b->tile_width, b->tile_height);

    drawArea.w = b->tile_width;
    drawArea.h = b->tile_height;

    freeSdlSurfaces(this);

    /* "empty" tiles with different edges and rotations */
    b->s_empty[0] = createScaledSurface(
	    b->t_empty, b->tile_width, b->tile_height);
    png = loadPngSurface(b->t_empty_1);
    b->s_empty[1] = scaleSurface(
	    png, b->tile_width, b->tile_height, 3);
    b->s_empty[2] = scaleSurface(
	    png, b->tile_width, b->tile_height, 2);
    b->s_empty[4] = scaleSurface(
	    png, b->tile_width, b->tile_height, 1);
    b->s_empty[8] = scaleSurface(
	    png, b->tile_width, b->tile_height, 0);
    SDL_FreeSurface(png);
    png = loadPngSurface(b->t_empty_2a);
    b->s_empty[8|1] = scaleSurface(
	    png, b->tile_width, b->tile_height, 3);
    b->s_empty[2|1] = scaleSurface(
	    png, b->tile_width, b->tile_height, 2);
    b->s_empty[4|2] = scaleSurface(
	    png, b->tile_width, b->tile_height, 1);
    b->s_empty[8|4] = scaleSurface(
	    png, b->tile_width, b->tile_height, 0);
    SDL_FreeSurface(png);
    png = loadPngSurface(b->t_empty_2f);
    b->s_empty[4|1] = scaleSurface(
	    png, b->tile_width, b->tile_height, 1);
    b->s_empty[8|2] = scaleSurface(
	    png, b->tile_width, b->tile_height, 0);
    SDL_FreeSurface(png);
    png = loadPngSurface(b->t_empty_3);
    b->s_empty[8|4|1] = scaleSurface(
	    png, b->tile_width, b->tile_height, 3);
    b->s_empty[8|2|1] = scaleSurface(
	    png, b->tile_width, b->tile_height, 2);
    b->s_empty[4|2|1] = scaleSurface(
	    png, b->tile_width, b->tile_height, 1);
    b->s_empty[8|4|2] = scaleSurface(
	    png, b->tile_width, b->tile_height, 0);
    SDL_FreeSurface(png);
    b->s_empty[8|4|2|1] = createScaledSurface(
	    b->t_empty_4, b->tile_width, b->tile_height);

    /* the four corners */
    png = loadPngSurface(b->t_corner);
    for (i=0; i<4; ++i)
	b->s_corner[i] = scaleSurface(
		png, b->tile_width, b->tile_height, i);
    SDL_FreeSurface(png);

    /* tiles for the items on the board */
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
    struct Board_impl *b;
    Resfile rf;
#ifndef SDL_IMG_OLD
    int imgflags;
#endif

    BASECTOR(Board, EHandler);

#ifndef SDL_IMG_OLD
    imgflags = IMG_Init(IMG_INIT_PNG);
    if (!(imgflags & IMG_INIT_PNG))
    {
	log_err("SDL_image library is missing PNG support!\n"
		"Try installing libpng and zlib.\n");
	mainApp->abort(mainApp);
    }
#endif

    b = XMALLOC(struct Board_impl, 1);
    memset(b, 0, sizeof(struct Board_impl));
    this->pimpl = b;

    ((EHandler)this)->handleEvent = &m_handleEvent;
    this->initVideo = &m_initVideo;
    this->loadLevel = &m_loadLevel;
    this->redraw = &m_redraw;
    this->draw = &m_draw;
    this->coordinatesToPixel = &m_coordinatesToPixel;
    this->entity = &m_entity;
    this->startMove = &m_startMove;
    this->getEmptyTile = &m_getEmptyTile;
    this->getEarthBaseTile = &m_getEarthBaseTile;
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
    rf->load(rf, "tile_empty_1", &(b->t_empty_1));
    rf->load(rf, "tile_empty_2a", &(b->t_empty_2a));
    rf->load(rf, "tile_empty_2f", &(b->t_empty_2f));
    rf->load(rf, "tile_empty_3", &(b->t_empty_3));
    rf->load(rf, "tile_empty_4", &(b->t_empty_4));
    rf->load(rf, "tile_corner", &(b->t_corner));
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

    this->initVideo(this);

    return this;
}

DTOR(Board)
{
    struct Board_impl *b = this->pimpl;

    internal_clear(this);

    freeSdlSurfaces(this);

    DELETE(Resource, b->t_empty);
    DELETE(Resource, b->t_empty_1);
    DELETE(Resource, b->t_empty_2a);
    DELETE(Resource, b->t_empty_2f);
    DELETE(Resource, b->t_empty_3);
    DELETE(Resource, b->t_empty_4);
    DELETE(Resource, b->t_corner);
    DELETE(Resource, b->t_earth);
    DELETE(Resource, b->t_wall);
    DELETE(Resource, b->t_rock);
    DELETE(Resource, b->t_cabbage);
    DELETE(Resource, b->t_willy);

#ifndef SDL_IMG_OLD
    IMG_Quit();
#endif

    XFREE(this->pimpl);
    BASEDTOR(EHandler);
}

