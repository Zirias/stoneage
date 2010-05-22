#include "erock.h"
#include "board.h"
#include "move.h"
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
	e = CAST(this, Entity);
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

    Entity e = CAST(this, Entity);
    if (e->m) return;
    e->m = NEW(Move);
    e->m->init(e->m, e, 0, 1, TR_Linear);
    e->b->startMove(e->b, e->m);
}

CTOR(ERock)
{
    BASECTOR(ERock, Entity);
    ((EHandler)this)->handleEvent = &m_handleEvent;
    Entity e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    this->fall = &m_fall;
    return this;
}

DTOR(ERock)
{
    BASEDTOR(Entity);
}

