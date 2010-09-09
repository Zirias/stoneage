#include "board.h"
#include "screen.h"
#include "app.h"
#include "level.h"
#include "move.h"

#include "erock.h"
#include "ecabbage.h"
#include "ewilly.h"

#define INVALID_XY(x, y) (x < 0 || x >= LVL_COLS || y < 0 || y >= LVL_ROWS)

struct Board_impl
{
    int numberOfMoves;

    SDL_TimerID moveticker;

    int num_rocks;
    int num_cabbages;
    int level;
    int levelStep;
    int paused;

    Entity entity[LVL_ROWS][LVL_COLS];
    ERock rock[LVL_ROWS * LVL_COLS];
    Entity emptyEntity;
};

static void checkRocks(Board this);
static void levelStart(Board this);

Uint32
createMoveTickEvent(Uint32 interval, void *param)
{
    Board b = CAST(param, Board);

    MoveTickEventData *data = XMALLOC(MoveTickEventData, 1);
    data->cancelMoves = 0;
    RaiseEvent(b->MoveTick, (Object)b, data);
    return interval;
}

static void
m_move(THIS, Entity e, int dx, int dy)
{
    METHOD(Board);

    Entity d;
    int lvl;
    int x, y;

    struct Board_impl *b = this->pimpl;

    b->entity[e->y][e->x] = 0;
    e->x += dx;
    e->y += dy;
    d = b->entity[e->y][e->x];
    b->entity[e->y][e->x] = e;

    /* destination occupied? */
    if (d)
    {
	/* found a cabbage? */
	if (CAST(d, ECabbage) && ! --(b->num_cabbages))
	{
	    lvl = b->level + 1;
	    if (lvl >= BUILTIN_LEVELS) lvl = 0;
	    this->loadLevel(this, lvl);
	    return;
	}

	/* delete destination entity */
	d->dispose(d);

	/* redraw region */
	for (x = e->x - 1; x < e->x + 2; ++x)
	    for (y = e->y - 1; y < e->y + 2; ++y)
		this->draw(this, x, y, 1);
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
	if (this->pimpl->levelStep)
	{
	    levelStart(this);
	}
	else
	{
	    checkRocks(this);
	}
    }
}

static void
Entity_MoveStarting(THIS, Object sender, void *data)
{
    METHOD(Board);

    MoveStartingEventData *msd = data;
    AddHandler(msd->m->Finished, (Object)this, &Move_Finished);
    
    if (!(this->pimpl->numberOfMoves++))
    {
	this->pimpl->moveticker = SDL_AddTimer(
		20, &createMoveTickEvent, this);
    }
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
	AddHandler(e->MoveStarting, b, &Entity_MoveStarting);
	r = CAST(e, ERock);
	if (r) b->pimpl->rock[b->pimpl->num_rocks++] = r;
	else if CAST(e, ECabbage) b->pimpl->num_cabbages++;
    }
}

static void
internal_clear(Board b)
{
    Entity *e;
    EWilly w;
    MoveTickEventData *data;
    Entity *end;

    if (b->pimpl->moveticker)
    {
	SDL_RemoveTimer(b->pimpl->moveticker);
	b->pimpl->moveticker = 0;
	CancelEvent(b->MoveTick);
	data = XMALLOC(MoveTickEventData, 1);
	data->cancelMoves = 1;
	RaiseEvent(b->MoveTick, (Object)b, data);
    }

    for (end = (Entity *)&(b->pimpl->entity) + LVL_COLS*LVL_ROWS,
	    e = (Entity *) &(b->pimpl->entity);
	    e != end;
	    ++e)
    {
	if(*e)
	{
	    (*e)->dispose(*e);
	}
	*e = 0;
    }
    
    b->pimpl->num_rocks = 0;
    b->pimpl->num_cabbages = 0;

    if (( w = getWilly() )) ((Entity)w)->dispose(w);
}

static void
m_draw(THIS, int x, int y, int refresh)
{
    METHOD(Board);

    Entity e;

    if (INVALID_XY(x, y)) return;

    e = this->pimpl->entity[y][x];
    if (!e || e->moving)
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
    SDL_Rect drawArea;
    Screen s;

    struct Board_impl *b = this->pimpl;
    SDL_Surface *sf = SDL_GetVideoSurface();

    if (b->level < 0) return;

    for (y = 0; y < LVL_ROWS; ++y)
	for (x = 0; x < LVL_COLS; ++x)
	    this->draw(this, x, y, 0);

    if (!refresh) return;

    s = getScreen();
    s->coordinatesToRect(s, 0, 0, LVL_COLS, LVL_ROWS, &drawArea);
    SDL_UpdateRect(sf, drawArea.x, drawArea.y, drawArea.w, drawArea.h);
}

static int
m_entity(THIS, int x, int y, Entity *e)
{
    METHOD(Board);

    if (INVALID_XY(x, y)) return -1;
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
	for (y = 0; y < LVL_ROWS; ++y) for (x = 0; x < LVL_COLS; ++x)
	{
	    s->coordinatesToRect(s, x, y, 1, 1, &drawArea);
	    SDL_BlitSurface(surface, 0, sf, &drawArea);
	}
	surface = (SDL_Surface *)s->getText(s, SATX_Paused);
	s->coordinatesToRect(s, LVL_COLS/2 - 4, LVL_ROWS/2, 9, 1, &drawArea);
	SDL_BlitSurface(surface, 0, sf, &drawArea);
	s->coordinatesToRect(s, 0, 0, LVL_COLS, LVL_ROWS, &drawArea);
	SDL_UpdateRect(sf, drawArea.x, drawArea.y, drawArea.w, drawArea.h);
    }
    else
    {
	if (b->numberOfMoves)
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
checkRocks(Board this)
{
    int i;
    ERock r;
    Entity e;
    EWilly w;
    int falling;

    struct Board_impl *b = this->pimpl;

    /* let willy finish his current move */
    w = getWilly();
    if (w && CAST(w, Entity)->moving) return;

    falling = 0;
    for (i = 0; i < b->num_rocks; ++i)
    {
	r = b->rock[i];
	e = CAST(r, Entity);
	if (e->y < LVL_ROWS - 1 && !b->entity[e->y+1][e->x])
	{
	    /* freeze willy and let rock fall */
	    falling = 1;
	    getWilly()->moveLock = 1;
	    r->fall(r);
	}
    }

    /* no rocks to fall -> unfreeze willy */
    if (!falling) getWilly()->moveLock = 0;
}

static void
levelStart(Board this)
{
    Level l;

    struct Board_impl *b = this->pimpl;

    internal_clear(this);
    l = NEW(Level);

    l->builtin(l, b->level);
    l->createEntities(l, this, (Entity *)&this->pimpl->entity);
    DELETE(Level, l);
    b->levelStep = 0;
    scanLevel(this);
    this->redraw(this, 1);
    checkRocks(this);
}

static void
m_loadLevel(THIS, int n)
{
    METHOD(Board);

    struct Board_impl *b = this->pimpl;

    if (n >= 0) b->level = n;
    b->levelStep = 1;
    if (!b->numberOfMoves) levelStart(this);
}

CTOR(Board)
{
    struct Board_impl *b;

    BASECTOR(Board, Object);

    b = XMALLOC(struct Board_impl, 1);
    memset(b, 0, sizeof(struct Board_impl));
    b->level = -1;
    b->levelStep = 0;
    b->emptyEntity = NEW(Entity);
    b->emptyEntity->init(b->emptyEntity, this, 0, 0);
    this->pimpl = b;

    this->loadLevel = &m_loadLevel;
    this->redraw = &m_redraw;
    this->draw = &m_draw;
    this->entity = &m_entity;
    this->move = &m_move;
    this->setPaused = &m_setPaused;

    this->MoveTick = CreateEvent();

    return this;
}

DTOR(Board)
{
    internal_clear(this);

    DestroyEvent(this->MoveTick);

    this->pimpl->emptyEntity->dispose(this->pimpl->emptyEntity);
    XFREE(this->pimpl);
    BASEDTOR(Object);
}

