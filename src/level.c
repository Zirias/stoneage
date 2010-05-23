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

static const uint8_t lvl_debug[][LVL_ROWS][LVL_COLS] =
{
    {
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,E,N,N,N,N,N,N,N,N,N,N,N,N,E,N,N,N,N,N,N,N,N,N,N,E,N,N,N,N,N},
	{N,N,N,N,N,N,E,N,N,N,N,E,N,N,N,N,E,N,N,N,E,N,E,N,N,E,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,E,N,N,N,N,N,N,N,N,N,E,N,N,N,E,N,N,N,N,N,N,N,N},
	{N,N,E,N,N,N,N,N,N,E,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,E,N,E,N,E,N,E,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,E,N,N,N,N,N,N,E,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,E,N,N,N,N,N,E,N,N,E,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,E,E,N,E,E,N,N,E,N,E,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,E,N,N,E,N,N,E,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N},
	{N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N}
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
	r = random() / (RAND_MAX / 10);
	if (r < 3) this->pimpl->data[y][x] = N;
	else if (r < 7) this->pimpl->data[y][x] = E;
	else this->pimpl->data[y][x] = r-5;
    }
    x = random() / (RAND_MAX / LVL_COLS);
    y = random() / (RAND_MAX / LVL_ROWS);
    this->pimpl->data[y][x] = H;
}

static void
m_debug(THIS, int num)
{
    METHOD(Level);

    memcpy(this->pimpl->data, lvl_debug[num],
	    LVL_ROWS * LVL_COLS * sizeof(uint8_t));
}

CTOR(Level)
{
    BASECTOR(Level, Object);
    this->pimpl = XMALLOC(struct Level_impl, 1);
    this->createEntities = &m_createEntities;
    this->random = &m_random;
    this->debug = &m_debug;
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

