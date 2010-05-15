#include "entity.h"

static void
m_init ARG(Board b, int x, int y)
{
    METHOD(Entity);

    this->b = b;
    this->x = x;
    this->y = y;
}

static void
m_dispose ARG()
{
    METHOD(Entity);
    DELETE(Entity, this);
}

CTOR(Entity)
{
    BASECTOR(Entity, Object);
    this->init = &m_init;
    this->dispose = &m_dispose;
    return this;
}

DTOR(Entity)
{
    BASEDTOR(Object);
}

