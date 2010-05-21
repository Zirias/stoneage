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

static const uint8_t lvl_debug[1][24][32] =
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
    uint8_t data[24][32];
};

static void
m_createEntities ARG(Board b, Entity *buffer)
{
    METHOD(Level);

    int x, y;
    Entity e;

    for (y=0; y<24; ++y) for (x=0; x<32; ++x)
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
m_random ARG()
{
    METHOD(Level);
    
    int x, y, r;

    for (y=0; y<24; ++y) for (x=0; x<32; ++x)
    {
	r = random() / (RAND_MAX / 10);
	if (r < 3) this->pimpl->data[y][x] = N;
	else if (r < 6) this->pimpl->data[y][x] = E;
	else this->pimpl->data[y][x] = r-5;
    }
    x = random() / (RAND_MAX / 32);
    y = random() / (RAND_MAX / 24);
    this->pimpl->data[y][x] = H;
}

static void
m_debug ARG(int num)
{
    METHOD(Level);

    memcpy(this->pimpl->data, lvl_debug[num], 32*24*sizeof(uint8_t));
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

