#include "ewilly.h"
#include "stoneage.h"
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
FUNC(parent_init)(THIS, Board b, int x, int y);

static void
Move_Finished(THIS, Object sender, void *data)
{
    METHOD(EWilly);

    int x, y;
    Entity e = CAST(this, Entity);

    for (x=e->x-1; x<=e->x+1; ++x) for (y=e->y-1; y<=e->y+1; ++y)
	e->b->draw(e->b, x, y, 1);
}

static void
Stoneage_MoveWilly(THIS, Object sender, void *data)
{
    METHOD(EWilly);
    
    Entity e, d, d1, d2;
    MoveWillyEventData *md;
   
    e = CAST(this, Entity);

    if (this->moveLock) return;	/* no moving allowed */
    if (!this->alive) return;	/* dead willy */
    if (e->m) return;		/* already moving */
    md = (MoveWillyEventData *)data;

    /* check destination coordinates */
    if (e->b->entity(e->b, e->x+md->x, e->y+md->y, &d) < 0)
	return;

    /* check diagonal neighbors if necessary */
    if (md->x && md->y)
    {
	e->b->entity(e->b, e->x+md->x, e->y, &d1);
	e->b->entity(e->b, e->x, e->y+md->y, &d2);

	/* both are walls or rocks? -> no move possible */
	if (d1 && d2 &&
		(CAST(d1, EWall) || CAST(d1, ERock)) &&
		(CAST(d2, EWall) || CAST(d2, ERock)))
	    return;
    }

    /* check destination field */
    if (!d || CAST(d, EEarth) || CAST(d, ECabbage))
    {
	e->m = NEW(Move);
	e->m->init(e->m, e, md->x, md->y, TR_Linear);
	AddHandler(e->m->Finished, this, &Move_Finished);
	e->b->startMove(e->b, e->m);
    }

    /* special case: pushing a rock */
    else if (!md->y && CAST(d, ERock) && !(d->m))
    {
	if (e->b->entity(e->b, e->x+2*md->x, e->y, &d1) < 0) return;
	if (!d1)
	{
	    e->m = NEW(Move);
	    e->m->init(e->m, e, md->x, 0, TR_Linear);
	    e->m->rel = MR_Master;
	    AddHandler(e->m->Finished, this, &Move_Finished);
	    e->b->startMove(e->b, e->m);
	    d->m = NEW(Move);
	    d->m->init(d->m, d, md->x, 0, TR_Linear);
	    d->m->rel = MR_Slave;
	    e->b->startMove(e->b, d->m);
	}
    }
}

static void
m_init(THIS, Board b, int x, int y)
{
    METHOD(EWilly);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getBackground = b->getEmptyBackground;
    this->moveLock = 0;
    this->alive = 1;
    instance = this;
    
    AddHandler(CAST(mainApp, Stoneage)->MoveWilly, this, &Stoneage_MoveWilly);
}

static void
m_dispose(THIS)
{
    METHOD(EWilly);
    DELETE(EWilly, this);
}

static const SDL_Surface *
m_getSurface(THIS)
{
    /* METHOD(EWilly); */

    Screen s = getScreen();
    return s->getTile(s, SATN_Willy, 0);
}

CTOR(EWilly)
{
    Entity e;

    BASECTOR(EWilly, Entity);

    e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    e->getSurface = &m_getSurface;
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
