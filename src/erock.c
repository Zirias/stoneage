#include "erock.h"
#include "ewilly.h"
#include "board.h"
#include "move.h"
#include "event.h"
#include "screen.h"

static void
Move_Finished(THIS, Object sender, void *data);

static void
createContinuationMove(ERock this)
{
    Entity e;
    Entity n;
    EWilly w;
    Move m;
    MoveStartingEventData *msd;

    w = 0;
    m = 0;
    e = (Entity) this;

    /* bottom of board */
    if (e->b->entity(e->b, e->x, e->y+1, &n) < 0) return;

    if (!n || n->moving || ( w = CAST(n, EWilly) )) {
	/* fall straight down */
	this->fall(this);
	if (w) w->alive = 0;
	return;
    }

    if (!CAST(n, ERock)) return;

    w = 0;
    /* rock below, check rolling off it's edge */
    if ((e->b->entity(e->b, e->x+1, e->y+1, &n) == 0) &&
	    (!n || n->moving || ( w = CAST(n, EWilly) )))
    {
	/* to the right ... */
	e->b->entity(e->b, e->x+1, e->y, &n);
	if (!n)
	{
	    m = NEW(Move);
	    m->init(m, e, 1, 1, TR_CircleX);
	    if (w) w->alive = 0;
	}
    }

    w = 0;
    if (!m && (e->b->entity(e->b, e->x-1, e->y+1, &n) == 0) &&
	    (!n || n->moving || ( w = CAST(n, EWilly) )))
    {
	/* or else to the left */
	e->b->entity(e->b, e->x-1, e->y, &n);
	if (!n)
	{
	    m = NEW(Move);
	    m->init(m, e, -1, 1, TR_CircleX);
	    if (w) w->alive = 0;
	}
    }

    /* found possible move -> start it */
    if (m)
    {
	e->moving = 1;
	AddHandler(m->Finished, this, &Move_Finished);
	msd = XMALLOC(MoveStartingEventData, 1);
	msd->m = m;
	RaiseEvent(e->MoveStarting, (Object)this, msd);
	m->start(m);
    }
}

static void
Move_Finished(THIS, Object sender, void *data)
{
    METHOD(ERock);

    Entity e;
    Move m;

    e = (Entity) this;
    m = (Move) sender;
    e->b->move(e->b, e, m->dx, m->dy);

    /* search for continuation move ONLY if rock was moving by itself
     * (not pushed by Willy)
     */
    if (e->moving)
    {
	e->moving = 0;
	createContinuationMove(this);
    }

    e->draw(e, 1);
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

    Entity e;
    Move m;
    MoveStartingEventData *msd;

    e = CAST(this, Entity);
    if (e->moving) return;
    m = NEW(Move);
    m->init(m, e, 0, 1, TR_Linear);
    e->moving = 1;
    AddHandler(m->Finished, this, &Move_Finished);
    msd = XMALLOC(MoveStartingEventData, 1);
    msd->m = m;
    RaiseEvent(e->MoveStarting, (Object)this, msd);
    m->start(m);
}

static const SDL_Surface *
m_getTile(THIS)
{
    /* METHOD(ERock); */

    Screen s = getScreen();
    return s->getTile(s, SATN_Rock, 0);
}

static void
m_attachMove(THIS, Move m)
{
    METHOD(ERock);

    Entity e;
    MoveStartingEventData *msd;

    e = (Entity)this;
    AddHandler(m->Finished, this, &Move_Finished);
    msd = XMALLOC(MoveStartingEventData, 1);
    msd->m = m;
    RaiseEvent(e->MoveStarting, (Object)e, msd);
}

CTOR(ERock)
{
    Entity e;

    BASECTOR(ERock, Entity);

    e = CAST(this, Entity);
    e->dispose = &m_dispose;
    e->getTile = &m_getTile;
    this->fall = &m_fall;
    this->attachMove = &m_attachMove;
    return this;
}

DTOR(ERock)
{
    BASEDTOR(Entity);
}

