#include "ewilly.h"
#include "board.h"
#include "move.h"
#include "event.h"
#include "ewall.h"
#include "eearth.h"
#include "ecabbage.h"
#include "erock.h"

static EWilly instance;

static void
FUNC(parent_init)(THIS, Board b, int x, int y);

static void
m_handleEvent(THIS, Event ev)
{
    METHOD(EWilly);

    Entity e, d, d1, d2;
    MoveData md;
    int x, y;

    e = CAST(this, Entity);

    if (ev->type == SAEV_Move)
    {
	if (this->moveLock) goto m_handleEvent_done;	/* no moving allowed */
	if (e->m) goto m_handleEvent_done;		/* already moving */
	md = (MoveData) ev->data;

	/* check destination coordinates */
	if (e->b->entity(e->b, e->x+md->x, e->y+md->y, &d) < 0)
	    goto m_handleEvent_done;

	/* check diagonal neighbors if necessary */
	if (md->x && md->y)
	{
	    e->b->entity(e->b, e->x+md->x, e->y, &d1);
	    e->b->entity(e->b, e->x, e->y+md->y, &d2);

	    /* both are walls or rocks? -> no move possible */
	    if (d1 && d2 &&
		    (CAST(d1, EWall) || CAST(d1, ERock)) &&
		    (CAST(d2, EWall) || CAST(d2, ERock)))
		goto m_handleEvent_done;
	}

	/* check destination field */
	if (!d || CAST(d, EEarth) || CAST(d, ECabbage))
	{
	    e->m = NEW(Move);
	    e->m->init(e->m, e, md->x, md->y, TR_Linear);
	    e->b->startMove(e->b, e->m);
	}

	/* special case: pushing a rock */
	else if (!md->y && CAST(d, ERock) && !(d->m))
	{
	    if (e->b->entity(e->b, e->x+2*md->x, e->y, &d1) < 0)
		goto m_handleEvent_done;
	    if (!d1)
	    {
		e->m = NEW(Move);
		e->m->init(e->m, e, md->x, 0, TR_Linear);
		e->m->rel = MR_Master;
		e->b->startMove(e->b, e->m);
		d->m = NEW(Move);
		d->m->init(d->m, d, md->x, 0, TR_Linear);
		d->m->rel = MR_Slave;
		e->b->startMove(e->b, d->m);
	    }
	}
    }
    else if (ev->type == SAEV_MoveFinished)
    {
	for (x=e->x-1; x<=e->x+1; ++x) for (y=e->y-1; y<=e->y+1; ++y)
	    e->b->draw(e->b, x, y, 1);
    }

m_handleEvent_done:
    DELETE(Event, ev);
}

static void
m_init(THIS, Board b, int x, int y)
{
    METHOD(EWilly);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getSurface = b->getWillyTile;
    e->getBaseSurface = b->getEmptyTile;
    this->moveLock = 0;
    instance = this;
}

static void
m_dispose(THIS)
{
    METHOD(EWilly);
    DELETE(EWilly, this);
}

CTOR(EWilly)
{
    Entity e;

    BASECTOR(EWilly, Entity);

    ((EHandler)this)->handleEvent = &m_handleEvent;
    e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    return this;
}

DTOR(EWilly)
{
    BASEDTOR(Entity);
}

EWilly
getWilly(void)
{
    return instance;
}

