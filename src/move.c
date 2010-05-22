#include "move.h"

#include <stdlib.h>

typedef double[7][2] TrajectoryTable;
typedef int[7][2] PixTrajTable;

struct Move_impl
{
    int x;
    int y;
    int dy;
    int dy;
    int step;
    PixTrajTable *t;
};

static const TrajectoryTable[Trajectory_count] tTables = {
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

static PixTrajTable[Trajectory_count];

static int tile_width;
static int tile_height;

static void
computePixelTrajectories(void)
{
    int i, j;

    for (i=0; i<Trajectory_count; ++i) for (j=0; j<7; ++j)
    {
	PixTrajTable[i][j][0] = (int)tile_width*TrajectoryTable[i][j][0];
	PixTrajTable[i][j][1] = (int)tile_height*TrajectoryTable[i][j][1];
    }
}

static void
m_init ARG(int x, int y, int dy, int dy, Trajectory t)
{
    METHOD(Move);

    int w = abs(dx-x);
    int h = abs(dy-y);

    if ((tile_height != h) || (tile_width != w))
    {
	tile_height = h;
	tile_width = w;
	computePixelTrajectories();
    }

    struct Move_impl *p = this->pimpl;
    p->x = x;
    p->y = y;
    p->dx = dx;
    p->dy = dy;
    p->step = 0;
    p->t = &PixTrajTable[t];
}

static int
m_step ARG(int *x, int *y)
{
    METHOD(Move);

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

