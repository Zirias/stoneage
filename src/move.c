#include "move.h"

#include <stdlib.h>

#include "entity.h"
#include "board.h"

struct Move_impl
{
    Entity e;
    int x;
    int y;
    int tx;
    int ty;
    int step;
    int (*t)[7][2];
};

static const double tTables[Trajectory_count][7][2] = {
    {
	{.125,.125},
	{.25,.25},
	{.375,.375},
	{.5,.5},
	{.625,.625},
	{.75,.75},
	{.875,.875}
    },
    {
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

static void
m_init(THIS, Entity e, int dx, int dy, Trajectory t)
{
    METHOD(Move);

    struct Move_impl *p = this->pimpl;

    p->e = e;
    e->b->coordinatesToPixel(e->b, e->x, e->y, &p->x, &p->y);
    e->b->coordinatesToPixel(e->b, e->x + dx, e->y + dy, &p->tx, &p->ty);

    p->step = 0;
    p->t = &(pTables[t]);

    this->dx = dx;
    this->dy = dy;
    this->next = 0;
    this->prev = 0;
    this->rel = MR_None;
}

static Entity
m_entity(THIS)
{
    METHOD(Move);
    return this->pimpl->e;
}

static int
m_step(THIS, Sint16 *x, Sint16 *y)
{
    METHOD(Move);

    int dx;
    int dy;

    struct Move_impl *p = this->pimpl;
    if (p->step == 7)
    {
	*x = p->tx;
	*y = p->ty;
	return 1;
    }
    
    dx = p->tx - p->x;
    dy = p->ty - p->y;
    if (dx>0) *x = p->x + (*p->t)[p->step][0];
    else if (dx<0) *x = p->x - (*p->t)[p->step][0];
    else *x = p->x;
    if (dy>0) *y = p->y + (*p->t)[p->step][1];
    else if (dy<0) *y = p->y - (*p->t)[p->step][1];
    else *y = p->y;
    p->step++;

    return 0;
}

CTOR(Move)
{
    BASECTOR(Move, Object);
    this->pimpl = XMALLOC(struct Move_impl, 1);
    this->init = &m_init;
    this->entity = &m_entity;
    this->step = &m_step;
    return this;
}

DTOR(Move)
{
    XFREE(this->pimpl);
    BASEDTOR(Object);
}

