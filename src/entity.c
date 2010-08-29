#include "entity.h"
#include "board.h"

static void
m_init(THIS, Board b, int x, int y)
{
    METHOD(Entity);

    this->b = b;
    this->x = x;
    this->y = y;
    this->m = 0;
}

static void
m_dispose(THIS)
{
    METHOD(Entity);
    DELETE(Entity, this);
}

CTOR(Entity)
{
    BASECTOR(Entity, EHandler);
    this->init = &m_init;
    this->dispose = &m_dispose;
    this->getSurface = 0;
    this->getBackground = 0;
    return this;
}

DTOR(Entity)
{
    BASEDTOR(EHandler);
}

