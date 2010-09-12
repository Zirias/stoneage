#include "ewilly.h"
#include "board.h"
#include "move.h"
#include "screen.h"
#include "event.h"
#include "ewall.h"
#include "eearth.h"
#include "ecabbage.h"
#include "erock.h"

static EWilly instance;

static void
Move_Finished(THIS, Object sender, void *data)
{
    METHOD(EWilly);

    Entity e;
    Move m;

    e = (Entity) this;
    m = (Move) sender;
    e->moving = 0;
    e->b->move(e->b, e, m->dx, m->dy);
}

static void
m_move(THIS, int dx, int dy)
{
    METHOD(EWilly);

    Entity e, d, d1, d2;
    ERock r;
    Move m, slave;
    MoveStartingEventData *msd;

    e = (Entity) this;

    if (this->moveLock) return;	/* no moving allowed */
    if (!this->alive) return;	/* dead willy */
    if (e->moving) return;	/* already moving */

    /* check destination coordinates */
    if (e->b->entity(e->b, e->x+dx, e->y+dy, &d) < 0)
	return;

    /* check diagonal neighbors if necessary */
    if (dx && dy)
    {
	e->b->entity(e->b, e->x+dx, e->y, &d1);
	e->b->entity(e->b, e->x, e->y+dy, &d2);

	/* both are walls or rocks? -> no move possible */
	if (d1 && d2 &&
		(CAST(d1, EWall) || CAST(d1, ERock)) &&
		(CAST(d2, EWall) || CAST(d2, ERock)))
	    return;
    }

    /* check destination field */
    if (!d || CAST(d, EEarth) || CAST(d, ECabbage))
    {
	m = NEW(Move);
	m->init(m, e, dx, dy, TR_Linear);
	AddHandler(m->Finished, this, &Move_Finished);
	e->moving = 1;
	msd = XMALLOC(MoveStartingEventData, 1);
	msd->m = m;
	RaiseEvent(e->MoveStarting, (Object)e, msd);
	m->start(m);
    }

    /* special case: pushing a rock */
    else if (!dy && (r = CAST(d, ERock)))
    {
	if (e->b->entity(e->b, e->x+2*dx, e->y, &d1) < 0)
	    return;
	if (!d1)
	{
	    slave = NEW(Move);
	    slave->init(slave, d, dx, 0, TR_Linear);
	    m = NEW(Move);
	    m->init(m, e, dx, 0, TR_Linear);
	    m->setSlave(m, slave);
	    r->attachMove(r, slave);
	    AddHandler(m->Finished, this, &Move_Finished);
	    e->moving = 1;
	    msd = XMALLOC(MoveStartingEventData, 1);
	    msd->m = m;
	    RaiseEvent(e->MoveStarting, (Object)e, msd);
	    m->start(m);
	}
    }
}

static void
m_dispose(THIS)
{
    METHOD(EWilly);
    DELETE(EWilly, this);
}

static const SDL_Surface *
m_getTile(THIS)
{
    /* METHOD(EWilly); */

    Screen s = getScreen();
    return s->getTile(s, SATN_Willy, 0);
}

CTOR(EWilly)
{
    Entity e;

    BASECTOR(EWilly, Entity);

    instance = this;
    e = CAST(this, Entity);
    e->dispose = &m_dispose;
    e->getTile = &m_getTile;
    this->move = &m_move;
    this->moveLock = 0;
    this->alive = 1;
    return this;
}

DTOR(EWilly)
{
    instance = 0;
    BASEDTOR(Entity);
}

EWilly
getWilly(void)
{
    return instance;
}
