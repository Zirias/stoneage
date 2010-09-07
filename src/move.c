#include "move.h"

#include <stdlib.h>
#include <SDL.h>

#include "entity.h"
#include "screen.h"
#include "board.h"

static void
Board_MoveTick(THIS, Object sender, void *data);

struct Move_impl
{
    Entity e;
    Move slave;
    SDL_Rect stepArea;
    SDL_Rect destArea;
    SDL_Rect dirtyArea;
    int x;
    int y;
    int step;
    int (*t)[7][2];
};

static const double tTables[Trajectory_count][7][2] = {
    {
	/* TR_Linear */
	{.125,.125},
	{ .25, .25},
	{.375,.375},
	{  .5,  .5},
	{.625,.625},
	{ .75, .75},
	{.875,.875}
    },
    {
	/* TR_CircleX */
	{.195,.020},
	{.383,.076},
	{.556,.169},
	{.707,.293},
	{.831,.443},
	{.924,.617},
	{.980,.805},
    }
};

static int pTables[Trajectory_count][7][2];

static int tr_tile_width;
static int tr_tile_height;

void
computeTrajectories(int tile_width, int tile_height)
{
    int i, j;
    if ((tr_tile_height != tile_height) || (tr_tile_width != tile_width))
    {
	tr_tile_height = tile_height;
	tr_tile_width = tile_width;
	for (i=0; i<Trajectory_count; ++i) for (j=0; j<7; ++j)
	{
	    pTables[i][j][0] = (int) tile_width * tTables[i][j][0];
	    pTables[i][j][1] = (int) tile_height * tTables[i][j][1];
	}
    }
}

static int
min(int a, int b)
{
    return (a<b)?a:b;
}

static void
m_init(THIS, Entity e, int dx, int dy, Trajectory t)
{
    METHOD(Move);

    Screen s = getScreen();
    struct Move_impl *m = this->pimpl;

    m->e = e;
    s->coordinatesToRect(s, e->x, e->y, 1, 1, &(m->stepArea));

    s->coordinatesToRect(s,
	    min(e->x, e->x + dx),
	    min(e->y, e->y + dy),
	    dx?2:1, dy?2:1, &(m->dirtyArea));
    
    m->x = m->stepArea.x;
    m->y = m->stepArea.y;

    m->step = 0;
    m->t = &(pTables[t]);

    this->dx = dx;
    this->dy = dy;
    m->slave = 0;
}

static void
m_setSlave(THIS, Move slave)
{
    METHOD(Move);

    this->pimpl->slave = slave;
}

static int
step(Move this)
{
    Screen s = getScreen();
    struct Move_impl *m = this->pimpl;

    if (m->step == 7)
    {
	s->coordinatesToRect(s, m->e->x + this->dx, m->e->y + this->dy,
		1, 1, &(m->stepArea));
	return 1;
    }
    
    m->stepArea.x = m->x + this->dx * (*m->t)[m->step][0];
    m->stepArea.y = m->y + this->dy * (*m->t)[m->step++][0];

    return 0;
}

static void
drawBackgrounds(Move this)
{
    struct Move_impl *m = this->pimpl;
    m->e->drawBackground(m->e, 0);
    if (this->dx)
    {
	m->e->b->draw(m->e->b, m->e->x + this->dx, m->e->y, 0); 
    }
    if (this->dy)
    {
	m->e->b->draw(m->e->b, m->e->x, m->e->y + this->dy, 0);
    }
    if (this->dx && this->dy)
    {
	m->e->b->draw(m->e->b, m->e->x + this->dx, m->e->y + this->dy, 0); 
    }
}

static void
drawTile(Move this)
{
    const SDL_Surface *tile;
    SDL_Surface *sf;

    struct Move_impl *m = this->pimpl;
    sf = SDL_GetVideoSurface();
    tile = m->e->getTile(m->e);

    SDL_BlitSurface((SDL_Surface *)tile, 0, sf, &(m->stepArea));
}

static void
refreshDirtyArea(Move this)
{
    struct Move_impl *m = this->pimpl;
    SDL_Surface *sf = SDL_GetVideoSurface();
    SDL_UpdateRect(sf, m->dirtyArea.x, m->dirtyArea.y,
	    m->dirtyArea.w, m->dirtyArea.h);
}

static void
Move_Finished(THIS, Object sender, void *data)
{
    METHOD(Move);

    RemoveHandler(this->pimpl->e->b->MoveTick, this, &Board_MoveTick);
    if (this->pimpl->slave) DELETE(Move, this->pimpl->slave);
    DELETE(Move, this);
}

static void
Board_MoveTick(THIS, Object sender, void *data)
{
    METHOD(Move);

    int done;
    struct Move_impl *m = this->pimpl;

    MoveTickEventData *mtd = data;
    if (mtd->cancelMoves)
    {
	Move_Finished(this, (Object)this, 0);
	return;
    }

    if (m->slave) drawBackgrounds(m->slave);
    drawBackgrounds(this);
    if (m->slave) step(m->slave);
    done = step(this);
    if (m->slave) drawTile(m->slave);
    drawTile(this);
    if (m->slave) refreshDirtyArea(m->slave);
    refreshDirtyArea(this);
    if (done)
    {
	AddHandler(this->Finished, this, Move_Finished);
	if (m->slave) RaiseEvent(m->slave->Finished, (Object)this, 0);
	RaiseEvent(this->Finished, (Object)this, 0);
    }
}

static void
m_start(THIS)
{
    METHOD(Move);

    Board b = this->pimpl->e->b;
    AddHandler(b->MoveTick, this, &Board_MoveTick);
}

CTOR(Move)
{
    BASECTOR(Move, Object);
    this->pimpl = XMALLOC(struct Move_impl, 1);
    this->init = &m_init;
    this->setSlave = &m_setSlave;
    this->start = &m_start;
    this->Finished = CreateEvent();
    return this;
}

DTOR(Move)
{
    DestroyEvent(this->Finished);
    XFREE(this->pimpl);
    BASEDTOR(Object);
}

