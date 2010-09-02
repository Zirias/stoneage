#include "erock.h"
#include "ewilly.h"
#include "board.h"
#include "move.h"
#include "event.h"
#include "screen.h"

static void
FUNC(parent_init)(THIS, Board b, int x, int y);

static void
Move_Finished(THIS, Object sender, void *data)
{
    METHOD(ERock);

    Entity e, n;
    EWilly w;

    e = CAST(this, Entity);
    w = 0;
    if (e->m)
    {
	if (e->m->finished)
	    DELETE(Move, e->m);
	else
	    return; /* new move already started */
    }

    /* bottom of board */
    if (e->b->entity(e->b, e->x, e->y+1, &n) < 0) return;

    if (!n || ( w = CAST(n, EWilly) )) {
	/* fall straight down */
	this->fall(this);
	if (w) w->alive = 0;
	return;
    }

    if (!CAST(n, ERock)) return;

    w = 0;
    /* rock below, check rolling off it's edge */
    if ((e->b->entity(e->b, e->x+1, e->y+1, &n) == 0) &&
	    (!n || ( w = CAST(n, EWilly) )))
    {
	/* to the right ... */
	e->b->entity(e->b, e->x+1, e->y, &n);
	if (!n)
	{
	    e->m = NEW(Move);
	    e->m->init(e->m, e, 1, 1, TR_CircleX);
	    if (w) w->alive = 0;
	}
    }
    w = 0;
    if (!e->m && (e->b->entity(e->b, e->x-1, e->y+1, &n) == 0) &&
	    (!n || ( w = CAST(n, EWilly) )))
    {
	/* or else to the left */
	e->b->entity(e->b, e->x-1, e->y, &n);
	if (!n)
	{
	    e->m = NEW(Move);
	    e->m->init(e->m, e, -1, 1, TR_CircleX);
	    if (w) w->alive = 0;
	}
    }

    /* found possible move -> start it */
    if (e->m)
    {
	e->b->startMove(e->b, e->m);
	AddHandler(e->m->Finished, this, &Move_Finished);
    }

}

static void
m_init(THIS, Board b, int x, int y)
{
    METHOD(ERock);

    Entity e = CAST(this, Entity);
    parent_init(e, b, x, y);

    e->getBackground = b->getEmptyBackground;
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
    if (e->m)
    {
	if (e->m->finished)
	    DELETE(Move, e->m);
	else
	    return; /* new move already started */
    }
    e->m = NEW(Move);
    e->m->init(e->m, e, 0, 1, TR_Linear);
    e->b->startMove(e->b, e->m);
    AddHandler(e->m->Finished, this, &Move_Finished);
}

static const SDL_Surface *
m_getSurface(THIS)
{
    /* METHOD(ERock); */

    Screen s = getScreen();
    return s->getTile(s, SATN_Rock, 0);
}

CTOR(ERock)
{
    Entity e;

    BASECTOR(ERock, Entity);

    e = CAST(this, Entity);
    parent_init = e->init;
    e->init = &m_init;
    e->dispose = &m_dispose;
    e->getSurface = &m_getSurface;
    this->fall = &m_fall;
    return this;
}

DTOR(ERock)
{
    BASEDTOR(Entity);
}

