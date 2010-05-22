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
    }
};

static int pTables[Trajectory_count][7][2];

static int tile_width;
static int tile_height;

static void
computePixelTrajectories(void)
{
    int i, j;

    for (i=0; i<Trajectory_count; ++i) for (j=0; j<7; ++j)
    {
	pTables[i][j][0] = (int) tile_width * tTables[i][j][0];
	pTables[i][j][1] = (int) tile_height * tTables[i][j][1];
    }
}

static void
m_init ARG(Entity e, int dx, int dy, Trajectory t)
{
    METHOD(Move);

    struct Move_impl *p = this->pimpl;

    p->e = e;
    e->b->coordinatesToPixel(e->b, e->x, e->y, &p->x, &p->y);
    e->b->coordinatesToPixel(e->b, e->x + dx, e->y + dy, &p->tx, &p->ty);

    int w = abs(p->tx-p->x);
    int h = abs(p->ty-p->y);

    if ((tile_height != h) || (tile_width != w))
    {
	tile_height = h;
	tile_width = w;
	computePixelTrajectories();
    }

    p->step = 0;
    p->t = &(pTables[t]);
}

static int
m_step ARG(int *x, int *y)
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
    *x = p->x + (int) dx * (*p->t)[p->step][0];
    *y = p->y + (int) dy * (*p->t)[p->step][1];
    p->step++;

    return 0;
}

CTOR(Move)
{
    BASECTOR(Move, Object);
    this->pimpl = XMALLOC(struct Move_impl, 1);
    this->init = &m_init;
    this->step = &m_step;
    return this;
}

DTOR(Move)
{
    XFREE(this->pimpl);
    BASEDTOR(Object);
}

