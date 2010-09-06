#include "board.h"
#include "screen.h"
#include "app.h"
#include "level.h"
#include "move.h"

#include "eearth.h"
#include "ewall.h"
#include "erock.h"
#include "ecabbage.h"
#include "ewilly.h"

struct Board_impl
{
    int numberOfMoves;

    SDL_TimerID moveticker;

    int num_rocks;
    int num_cabbages;
    int level;
    int paused;

    SDL_Rect drawArea;
    int off_x;
    int off_y;

    Entity entity[LVL_ROWS][LVL_COLS];
    ERock rock[LVL_ROWS * LVL_COLS];
    Entity emptyEntity;
};

static void checkRocks(Board b);

Uint32
createMoveTickEvent(Uint32 interval, void *param)
{
    Board b = CAST(param, Board);

    RaiseEvent(b->MoveTick, (Object)b, 0);
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
Move_Finished(THIS, Object sender, void *data)
{
    METHOD(Board);

    if (!(--this->pimpl->numberOfMoves))
    {
	SDL_RemoveTimer(this->pimpl->moveticker);
	this->pimpl->moveticker = 0;
    }
}

static void
Entity_MoveStarting(THIS, Object sender, void *data)
{
    METHOD(Board);

    Move m = data;
    AddHandler(m->Finished, (Object)this, &Move_Finished);
    
    if (!(this->pimpl->numberOfMoves++))
    {
	this->pimpl->moveticker = SDL_AddTimer(
		20, &createMoveTickEvent, this);
    }
}

static void
moveStep(Board this, Move m)
{
    int done;
    int tw, th;
    SDL_Rect dirty;
    Entity e;
    Entity d;
    
    Screen s = getScreen();
    SDL_Surface *sf = SDL_GetVideoSurface();
    struct Board_impl *b = this->pimpl;

    e = m->entity(m);
    tw = b->drawArea.w;
    th = b->drawArea.h;
    s->coordinatesToPixel(s, e->x, e->y, &dirty.x, &dirty.y);
    dirty.w = tw;
    dirty.h = th;

    if (m->rel != MR_Slave)
	this->draw(this, e->x, e->y, 0);
    if (m->rel != MR_Master)
    {
	if (m->dx)
	{
	    this->draw(this, e->x + m->dx, e->y, 0);
	    if (m->dx < 0) dirty.x -= tw;
	    dirty.w += tw;
	}
	if (m->dy)
	{
	    this->draw(this, e->x, e->y + m->dy, 0);
	    if (m->dy < 0) dirty.y -= th;
	    dirty.h += th;
	}
	if (m->dx && m->dy) this->draw(this, e->x + m->dx, e->y + m->dy, 0);
    }

    done = m->step(m, &(b->drawArea.x), &(b->drawArea.y));
    SDL_BlitSurface((SDL_Surface *)e->getSurface(e),
	    0, sf, &(b->drawArea));
    SDL_UpdateRects(sf, 1, &dirty);

    if (done)
    {
	if (m->prev)
	{
	    m->prev->next = m->next;
	}
	else
	{
	    b->movelist = m->next;
	}
	if (m->next)
	{
	    m->next->prev = m->prev;
	}
	b->entity[e->y][e->x] = 0;
	e->x += m->dx;
	e->y += m->dy;
	e->m = 0;
	d = b->entity[e->y][e->x];
	if (d)
	{
	    if (CAST(d, ECabbage))
	    {
		b->num_cabbages--;
	    }
	    if (!CAST(d, EWilly)) d->dispose(d);
	}
	b->entity[e->y][e->x] = e;

	if (m->rel != MR_Slave)
	{
	    /* don't give the slave of a move any events */

	    /*
	    ev = NEW(Event);
	    ev->sender = CAST(this, Object);
	    ev->handler = CAST(e, EHandler);
	    ev->type = SAEV_MoveFinished;
	    RaiseEvent(ev);
	    */
	}

	DELETE(Move, m);

	checkRocks(this);

	if (!b->movelist)
	{
	    /* all moves finished, unfreeze willy */
	    getWilly()->moveLock = 0;
	}
    }

    if (!b->num_cabbages) {
	b->level++;
	if (b->level >= BUILTIN_LEVELS) b->level = 0;
	this->loadLevel(this, -1);
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
	CancelEvent(b->MoveTick);
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
	    (*e)->dispose(*e);
	}
	*e = 0;
    }
    
    b->pimpl->num_rocks = 0;
    b->pimpl->num_cabbages = 0;

    if (( w = getWilly() )) DELETE(EWilly, w);
}

/*
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
*/

static void
m_draw(THIS, int x, int y, int refresh)
{
    METHOD(Board);

    Entity e;

    if (x<0||x>LVL_COLS||y<0||y>LVL_ROWS)
	return;
    e = this->pimpl->entity[y][x];
    if (!e)
    {
	e = this->pimpl->emptyEntity;
	e->x = x;
	e->y = y;
    }
    e->draw(e, refresh);
}

static void
m_redraw(THIS, int refresh)
{
    METHOD(Board);
    int x, y;

    struct Board_impl *b = this->pimpl;
    SDL_Surface *sf = SDL_GetVideoSurface();

    if (b->level < 0) return;

    for (y = 0; y < LVL_ROWS; ++y)
	for (x = 0; x < LVL_COLS; ++x)
	    this->draw(this, x, y, 0);

    if (!refresh) return;

    SDL_UpdateRect(sf, b->off_x, b->off_y,
	    b->drawArea.w * LVL_COLS, b->drawArea.h * LVL_ROWS);
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
m_setPaused(THIS, int paused)
{
    METHOD(Board);

    SDL_Surface *surface;
    int x, y;
    SDL_Rect drawArea;

    struct Board_impl *b = this->pimpl;
    Screen s = getScreen();
    SDL_Surface *sf = SDL_GetVideoSurface();

    if (paused)
    {
	b->paused = 1;
	if (b->moveticker)
	{
	    SDL_RemoveTimer(b->moveticker);
	    b->moveticker = 0;
	}
	surface = (SDL_Surface *)s->getTile(s, SATN_Empty, 0);
	drawArea.w = b->drawArea.w;
	drawArea.h = b->drawArea.h;
	for (y = 0; y < LVL_ROWS; ++y) for (x = 0; x < LVL_COLS; ++x)
	{
	    s->coordinatesToPixel(s, x, y,
		    &drawArea.x, &drawArea.y);
	    SDL_BlitSurface(surface, 0, sf, &drawArea);
	}
	surface = (SDL_Surface *)s->getText(s, SATX_Paused);
	drawArea.w *= 9;
	s->coordinatesToPixel(s, LVL_COLS/2 - 4, LVL_ROWS/2,
		&drawArea.x, &drawArea.y);
	SDL_BlitSurface(surface, 0, sf, &drawArea);
	SDL_UpdateRect(sf, b->off_x, b->off_y,
		b->drawArea.w * LVL_COLS, b->drawArea.h * LVL_ROWS);
    }
    else
    {
	if (b->movelist)
	{
	    b->moveticker = SDL_AddTimer(20, &createMoveTickEvent, this);
	}
	else
	{
	    checkRocks(this);
	}
	b->paused = 0;

	this->redraw(this, 1);
    }
}

static void
m_startMove(THIS, Move m)
{
    METHOD(Board);

    if (this->pimpl->paused) return;

    if (this->pimpl->movelist) 
    {
	this->pimpl->movelist->prev = m;
	m->next = this->pimpl->movelist;
    }
    this->pimpl->movelist = m;

    if (!this->pimpl->moveticker)
    {
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

static void
m_getEarthBackground(THIS, int x, int y, void *buf)
{
    /* METHOD(Board); */

    Screen s = getScreen();

    BlitSequence bs = (BlitSequence)buf;
    bs->n = 1;
    bs->blits[0] = s->getTile(s, SATN_Earth, 0);
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
    this->redraw(this, 1);
    checkRocks(this);
}

static void
m_setGeometry(THIS, int width, int height, int off_x, int off_y)
{
    METHOD(Board);

    struct Board_impl *b = this->pimpl;

    if (width != b->drawArea.w || height != b->drawArea.h)
    {
	b->drawArea.w = width;
	b->drawArea.h = height;
	computeTrajectories(width, height);
    }
    b->off_x = off_x;
    b->off_y = off_y;
}

CTOR(Board)
{
    struct Board_impl *b;

    BASECTOR(Board, Object);

    b = XMALLOC(struct Board_impl, 1);
    memset(b, 0, sizeof(struct Board_impl));
    b->level = -1;
    b->emptyEntity = NEW(Entity);
    b->emptyEntity->init(b->emptyEntity, this, 0, 0);
    this->pimpl = b;

    this->setGeometry = &m_setGeometry;
    this->loadLevel = &m_loadLevel;
    this->redraw = &m_redraw;
    this->draw = &m_draw;
    this->entity = &m_entity;
    this->startMove = &m_startMove;
    this->setPaused = &m_setPaused;
    this->getEmptyBackground = &m_getEmptyBackground;
    this->getEarthBackground = &m_getEarthBackground;

    return this;
}

DTOR(Board)
{
    internal_clear(this);

    DELETE(this->pimpl->emptyEntity);
    XFREE(this->pimpl);
    BASEDTOR(Object);
}

