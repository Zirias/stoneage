#include <stdint.h>
#include <string.h>

#include "level.h"
#include "resource.h"
#include "board.h"

#include "entity.h"
#include "eearth.h"
#include "ewall.h"
#include "erock.h"
#include "ecabbage.h"
#include "ewilly.h"

#define N 0	/* nothing */
#define E 1	/* earth */
#define W 2	/* wall */
#define R 3	/* rock */
#define C 4	/* cabbage */
#define H 5	/* hero (aka willy) */

#define BUILTIN_LEVELS 1

static const uint8_t lvl_data[BUILTIN_LEVELS][LVL_ROWS][LVL_COLS] =
{
    {
	{R,C,E,R,W,R,N,N,E,E,W,E,E,N,E,E,E,E,E,E,R,E,E,E,N,N,E,E,E,E,R},
	{R,W,W,E,W,C,R,W,W,E,W,W,R,N,W,E,E,W,W,E,E,E,W,W,E,E,R,E,R,E,R},
	{R,N,N,N,W,E,E,W,R,E,W,E,E,W,E,W,R,W,R,W,E,W,W,W,N,E,R,E,R,E,R},
	{E,N,N,N,W,R,E,E,W,N,W,E,E,W,N,W,R,W,C,W,E,W,N,N,N,E,R,E,R,E,C},
	{E,E,R,E,W,R,E,W,W,N,W,W,R,E,W,R,R,W,E,W,E,E,W,W,E,E,R,E,R,E,N},
	{E,W,E,W,W,W,N,E,E,N,E,E,E,E,E,R,R,E,E,E,N,E,E,E,E,E,R,R,R,R,N},
	{E,W,E,W,R,W,W,W,W,W,W,E,E,E,E,W,W,W,W,W,E,E,R,R,R,E,R,R,R,R,N},
	{N,W,E,W,R,W,E,N,E,E,W,E,R,E,R,N,N,N,N,N,R,E,R,R,R,E,R,R,R,E,N},
	{N,W,C,W,R,W,E,W,W,E,W,E,R,E,W,R,N,N,N,R,W,E,R,C,R,E,R,R,E,N,N},
	{N,W,W,W,R,W,E,R,W,E,W,E,R,E,W,W,N,E,E,W,W,E,R,R,R,E,R,E,N,N,R},
	{E,R,R,R,R,W,N,E,W,N,W,E,R,E,E,W,N,E,E,W,R,E,E,E,E,E,R,N,N,R,R},
	{R,E,E,E,R,W,N,E,W,E,W,E,R,E,E,W,C,W,C,W,R,E,E,E,E,E,R,N,R,R,R},
	{R,R,R,R,R,W,R,C,W,E,E,E,R,E,E,W,W,W,W,W,E,E,E,W,E,E,W,N,E,E,E},
	{W,W,W,W,R,W,W,W,W,R,E,E,E,E,E,R,R,N,N,R,E,E,E,W,E,W,N,N,E,E,E},
	{W,E,E,E,C,E,E,E,W,R,E,W,E,W,E,W,W,W,E,W,W,E,E,W,W,N,N,N,N,N,N},
	{W,E,E,E,E,E,E,E,W,R,E,W,E,W,E,W,N,W,N,W,E,W,E,W,W,N,N,N,N,N,N},
	{W,E,E,E,E,E,E,E,W,R,E,W,C,W,E,W,N,W,N,W,R,W,E,W,N,W,N,N,N,N,N},
	{W,H,E,E,E,E,E,E,E,E,E,E,W,E,E,W,W,W,C,W,R,W,R,W,N,E,W,C,W,C,C}
    }
};

struct Level_impl
{
    uint8_t data[LVL_ROWS][LVL_COLS];
};

static void
m_createEntities(THIS, Board b, Entity *buffer)
{
    METHOD(Level);

    int x, y;
    Entity e;

    for (y=0; y<LVL_ROWS; ++y) for (x=0; x<LVL_COLS; ++x)
    {
	switch(this->pimpl->data[y][x])
	{
	    case E:
		e = (Entity)NEW(EEarth);
		break;
	    case W:
		e = (Entity)NEW(EWall);
		break;
	    case R:
		e = (Entity)NEW(ERock);
		break;
	    case C:
		e = (Entity)NEW(ECabbage);
		break;
	    case H:
		e = (Entity)NEW(EWilly);
		break;
	    default:
		e = 0;
	}
	if (e) e->init(e, b, x, y);
	*buffer++ = e;
    }
}

static void
m_random(THIS)
{
    METHOD(Level);
    
    int x, y, r;

    for (y=0; y<LVL_ROWS; ++y) for (x=0; x<LVL_COLS; ++x)
    {
	r = rand() / (RAND_MAX / 10);
	if (r < 3) this->pimpl->data[y][x] = N;
	else if (r < 7) this->pimpl->data[y][x] = E;
	else this->pimpl->data[y][x] = r-5;
    }
    x = rand() / (RAND_MAX / LVL_COLS);
    y = rand() / (RAND_MAX / LVL_ROWS);
    this->pimpl->data[y][x] = H;
}

static void
m_builtin(THIS, int num)
{
    METHOD(Level);

    memcpy(this->pimpl->data, lvl_data[num],
	    LVL_ROWS * LVL_COLS * sizeof(uint8_t));
}

CTOR(Level)
{
    BASECTOR(Level, Object);
    this->pimpl = XMALLOC(struct Level_impl, 1);
    this->createEntities = &m_createEntities;
    this->random = &m_random;
    this->builtin = &m_builtin;
    return this;
}

DTOR(Level)
{
    XFREE(this->pimpl);
    BASEDTOR(Object);
}

#undef N
#undef E
#undef W
#undef R
#undef C
#undef H

