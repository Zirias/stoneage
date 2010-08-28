#include "board.h"
#include "screen.h"
#include "app.h"
#include "event.h"
#include "level.h"
#include "move.h"

#include "eearth.h"
#include "ewall.h"
#include "erock.h"
#include "ecabbage.h"
#include "ewilly.h"

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

    Move movelist;

    SDL_TimerID moveticker;

    int num_rocks;
    int num_cabbages;
    int level;

    SDL_Rect drawArea;
    int off_x;
    int off_y;

    Entity entity[LVL_ROWS][LVL_COLS];
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

static void
scanLevel(Board b)
{
    int x, y;
    Entity e;
    ERock r;

    b->pimpl->num_rocks = 0;
    b->pimpl->num_cabbages = 0;
    for (y=0; y<LVL_ROWS; ++y) for (x=0; x<LVL_COLS; ++x)
    {
	e = b->pimpl->entity[y][x];
	if (!e) continue;
	r = CAST(e, ERock);
	if (r) b->pimpl->rock[b->pimpl->num_rocks++] = r;
	else if CAST(e, ECabbage) b->pimpl->num_cabbages++;
    }
}

static void
moveStep(Board b, Move m)
{
    int done;
    int tw, th;
    SDL_Rect dirty;
    Entity e;
    Entity d;
    Event ev;

    e = m->entity(m);
    tw = b->pimpl->tile_width;
    th = b->pimpl->tile_height;
    b->coordinatesToPixel(b, e->x, e->y, &dirty.x, &dirty.y);
    dirty.w = tw;
    dirty.h = th;

    if (m->rel != MR_Slave)
	b->draw(b, e->x, e->y, 0);
    if (m->rel != MR_Master)
    {
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
    }

    done = m->step(m, &drawArea.x, &drawArea.y);
    SDL_BlitSurface((SDL_Surface *)e->getSurface(e),
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
	d = b->pimpl->entity[e->y][e->x];
	if (d)
	{
	    if (CAST(d, ECabbage))
	    {
		b->pimpl->num_cabbages--;
	    }
	    if (!CAST(d, EWilly)) d->dispose(d);
	}
	b->pimpl->entity[e->y][e->x] = e;

	if (m->rel != MR_Slave)
	{
	    /* don't give the slave of a move any events */

	    ev = NEW(Event);
	    ev->sender = CAST(b, Object);
	    ev->handler = CAST(e, EHandler);
	    ev->type = SAEV_MoveFinished;
	    RaiseEvent(ev);
	}

	DELETE(Move, m);

	checkRocks(b);

	if (!b->pimpl->movelist)
	{
	    /* all moves finished, unfreeze willy */
	    getWilly()->moveLock = 0;
	}
    }

    if (!b->pimpl->num_cabbages) {
	b->pimpl->level++;
	if (b->pimpl->level >= BUILTIN_LEVELS) b->pimpl->level = 0;
	b->loadLevel(b, -1);
    }
}

static void
internal_clear(Board b)
{
    Move m, next;
    Entity *e;
    EWilly w;
    Entity *end = (Entity *)&(b->pimpl->entity) + LVL_COLS*LVL_ROWS;

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
    b->pimpl->num_cabbages = 0;

    if (( w = getWilly() )) DELETE(EWilly, w);
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

    Screen s = getScreen();
    if (s->coordinatesToPixel(this, x, y, &drawArea.x, &drawArea.y) < 0)
	return;

    e = this->pimpl->entity[y][x];

    screen = this->pimpl->screen;

    drawBase = 0;
    tile = 0;

    if (e)
    {
	if (e->getBackground)
	{
	    e->getBackground(this, x, y, &base);
	    drawBase = 1;
	}
	tile = e->getSurface(e);
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
    int x, y;

    struct Board_impl *b = this->pimpl;

    for (y = 0; y < LVL_ROWS; ++y)
	for (x = 0; x < LVL_COLS; ++x)
	    this->draw(this, x, y, 0);

    SDL_UpdateRect(b->screen, b->off_x, b->off_y,
	    drawArea.w * LVL_COLS, drawArea.h * LVL_ROWS);
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
    EWilly w;

    /* let willy finish his current move */
    w = getWilly();
    if (w && CAST(w, Entity)->m) return;

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
    unsigned int neighbors = 0;
    if ((y > 0) && isSolidTile(this, x, y-1)) neighbors += 8;
    if ((x < LVL_COLS-1) && isSolidTile(this, x+1, y)) neighbors += 4;
    if ((y < LVL_ROWS-1) && isSolidTile(this, x, y+1)) neighbors += 2;
    if ((x > 0) && isSolidTile(this, x-1, y)) neighbors += 1;
    return neighbors;
}

static void
m_getEmptyBackground(THIS, int x, int y, void *buf)
{
    METHOD(Board);
    
    Screen s = getScreen();
    BlitSequence bs = (BlitSequence)buf;
    unsigned int n = calculateNeighbors(this, x, y);

    bs->n = 0;
    bs->blits[bs->n++] = s->getTile(
	    emptyTileTable[n][0], emptyTileTable[n][1]);
    if (!(n&(8|4)) && (y>0) && (x<LVL_COLS-1)
	    && isSolidTile(this, x+1, y-1))
	bs->blits[bs->n++] = s->getTile(SATN_Corner, 0);
    if (!(n&(4|2)) && (y<LVL_ROWS-1) && (x<LVL_COLS-1)
	    && isSolidTile(this, x+1, y+1))
	bs->blits[bs->n++] = s->getTile(SATN_Corner, 1);
    if (!(n&(2|1)) && (y<LVL_ROWS-1) && (x>0)
	    && isSolidTile(this, x-1, y+1))
	bs->blits[bs->n++] = s->getTile(SATN_Corner, 2);
    if (!(n&(8|1)) && (y>0) && (x>0)
	    && isSolidTile(this, x-1, y-1))
	bs->blits[bs->n++] = s->getTile(SATN_Corner, 3);
}

static void
m_getEarthBackground(THIS, int x, int y, void *buf)
{
    METHOD(Board);

    Screen s = getScreen();

    BlitSequence bs = (BlitSequence)buf;
    bs->n = 1;
    bs->blits[0] = s->getTile(SATN_Earth, 0);
}

static void
m_loadLevel(THIS, int n)
{
    Level l;
    METHOD(Board);

    if (n < 0) n = this->pimpl->level;
    internal_clear(this);
    l = NEW(Level);
    l->builtin(l, n);
    l->createEntities(l, this, (Entity *)&this->pimpl->entity);
    DELETE(Level, l);
    this->pimpl->level = n;
    scanLevel(this);
    this->redraw(this);
    checkRocks(this);
}

static void
m_setTileSize(THIS, int tile_width, int tile_height)
{
    METHOD(Board);

    struct Board_impl *b = this->pimpl;

    if (tile_width != b->drawArea.w || tile_height != b->drawArea.h)
    {
	b->drawArea.w = tile_width;
	b->drawArea.h = tile_height;
	computeTrajectories(b->drawArea.w, b->drawArea.h);
    }
}

CTOR(Board)
{
    struct Board_impl *b;
    Resfile rf;

    BASECTOR(Board, EHandler);

    b = XMALLOC(struct Board_impl, 1);
    memset(b, 0, sizeof(struct Board_impl));
    this->pimpl = b;

    ((EHandler)this)->handleEvent = &m_handleEvent;
    this->setTileSize = &m_setTileSize;
    this->loadLevel = &m_loadLevel;
    this->redraw = &m_redraw;
    this->draw = &m_draw;
    this->entity = &m_entity;
    this->startMove = &m_startMove;
    this->getEmptyBackground = &m_getEmptyBackground;
    this->getEarthBackground = &m_getEarthBackground;

    this->initVideo(this);

    return this;
}

DTOR(Board)
{
    struct Board_impl *b = this->pimpl;

    internal_clear(this);

    XFREE(this->pimpl);
    BASEDTOR(EHandler);
}

