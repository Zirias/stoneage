#include "ewilly.h"
#include "board.h"
#include "move.h"
#include "screen.h"
#include "stoneage.h"
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
    PositionChangedEventData *pcd;

    e = (Entity) this;
    m = (Move) sender;
    pcd = XMALLOC(PositionChangedEventData, 1);
    pcd->from_x = e->x;
    pcd->from_y = e->y;
    pcd->to_x = e->x + m->dx;
    pcd->to_y = e->y + m->dy;
    e->x = pcd->to_x;
    e->y = pcd->to_y;
    RaiseEvent(e->PositionChanged, (Object)this, pcd);

    e->moving = 0;
}

static void
Stoneage_MoveWilly(THIS, Object sender, void *data)
{
    METHOD(EWilly);

    Entity e, d, d1, d2;
    ERock r;
    Move m, slave;
    MoveStartingEventData *msd;
    MoveWillyEventData *md;

    e = CAST(this, Entity);

    if (this->moveLock) return;	/* no moving allowed */
    if (!this->alive) return;	/* dead willy */
    if (e->moving) return;	/* already moving */

    md = data;

    /* check destination coordinates */
    if (e->b->entity(e->b, e->x+md->dx, e->y+md->dy, &d) < 0)
	return;

    /* check diagonal neighbors if necessary */
    if (md->dx && md->dy)
    {
	e->b->entity(e->b, e->x+md->dx, e->y, &d1);
	e->b->entity(e->b, e->x, e->y+md->dy, &d2);

	/* both are walls or rocks? -> no move possible */
	if (d1 && d2 && !d1->moving && !d2->moving &&
		(CAST(d1, EWall) || CAST(d1, ERock)) &&
		(CAST(d2, EWall) || CAST(d2, ERock)))
	    return;
    }

    /* check destination field */
    if (!d || d->moving || CAST(d, EEarth) || CAST(d, ECabbage))
    {
	m = NEW(Move);
	m->init(m, e, md->dx, md->dy, TR_Linear);
	AddHandler(m->Finished, this, &Move_Finished);
	e->moving = 1;
	msd = XMALLOC(MoveStartingEventData, 1);
	msd->m = m;
	RaiseEvent(e->MoveStarting, (Object)e, msd);
	m->start(m);
    }

    /* special case: pushing a rock */
    else if (!md->dy && (r = CAST(d, ERock)))
    {
	if (e->b->entity(e->b, e->x+2*md->dx, e->y, &d1) < 0)
	    return;
	if (!d1 || d1->moving)
	{
	    slave = NEW(Move);
	    slave->init(slave, d, md->dx, 0, TR_Linear);
	    m = NEW(Move);
	    m->init(m, e, md->dx, 0, TR_Linear);
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
    Stoneage s;

    BASECTOR(EWilly, Entity);

    instance = this;
    e = CAST(this, Entity);
    e->dispose = &m_dispose;
    e->getTile = &m_getTile;
    this->moveLock = 0;
    this->alive = 1;
    s = (Stoneage)mainApp;
    AddHandler(s->MoveWilly, this, &Stoneage_MoveWilly);
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
