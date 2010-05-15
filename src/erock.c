#include "erock.h"
#include "board.h"
#include "event.h"
#include "ehandler.h"

static void
FUNC(parent_init) ARG(Board b, int x, int y);

static void
m_handleEvent ARG(Event ev)
{
    METHOD(ERock);

    Entity e;

    if (ev->type == SAEV_MoveFinished)
    {
	this->falling = 0;
	e = ((Entity)this);
	if (e->b->isEmpty(e->b, e->x, e->y+1)) this->fall(this);
    }

    DELETE(Event, ev);
}

static void
m_init ARG(Board b, int x, int y)
{
    METHOD(ERock);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getSurface = b->getRockTile;
    e->getBaseSurface = b->getEmptyTile;
}

static void
m_dispose ARG()
{
    METHOD(ERock);
    DELETE(ERock, this);
}

static void
m_fall ARG()
{
    METHOD(ERock);

    if (this->falling) return;
    this->falling = 1;
    Entity e = CAST(this, Entity);
    e->b->startMove(e->b, e, 0, 1);
}

CTOR(ERock)
{
    BASECTOR(ERock, Entity);
    ((EHandler)this)->handleEvent = &m_handleEvent;
    Entity e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    this->falling = 0;
    this->fall = &m_fall;
    return this;
}

DTOR(ERock)
{
    BASEDTOR(Entity);
}

