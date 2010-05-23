#include "erock.h"
#include "board.h"
#include "move.h"
#include "event.h"
#include "ehandler.h"

static void
FUNC(parent_init)(THIS, Board b, int x, int y);

static void
m_handleEvent(THIS, Event ev)
{
    METHOD(ERock);

    Entity e, n;

    if (ev->type == SAEV_MoveFinished)
    {
	e = CAST(this, Entity);
	if (e->m) goto m_handleEvent_done; /* new move already started */

	if (e->b->entity(e->b, e->x, e->y+1, &n) < 0) goto m_handleEvent_done;
	if (!n) {
	    this->fall(this);
	    goto m_handleEvent_done;
	}
	if (!CAST(n, ERock)) goto m_handleEvent_done;
	if ((e->b->entity(e->b, e->x+1, e->y+1, &n) == 0) && (!n))
	{
	    e->b->entity(e->b, e->x+1, e->y, &n);
	    if (!n)
	    {
		e->m = NEW(Move);
		e->m->init(e->m, e, 1, 1, TR_CircleX);
	    }
	}
	else if ((e->b->entity(e->b, e->x-1, e->y+1, &n) == 0) && (!n))
	{
	    e->b->entity(e->b, e->x-1, e->y, &n);
	    if (!n)
	    {
		e->m = NEW(Move);
		e->m->init(e->m, e, -1, 1, TR_CircleX);
	    }
	}
	if (e->m)
	    e->b->startMove(e->b, e->m);
    }

m_handleEvent_done:
    DELETE(Event, ev);
}

static void
m_init(THIS, Board b, int x, int y)
{
    METHOD(ERock);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getSurface = b->getRockTile;
    e->getBaseSurface = b->getEmptyTile;
}

static void
m_dispose(THIS)
{
    METHOD(ERock);
    DELETE(ERock, this);
}

static void
m_fall(THIS)
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
    Entity e;

    BASECTOR(ERock, Entity);

    ((EHandler)this)->handleEvent = &m_handleEvent;
    e = CAST(this, Entity);
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

