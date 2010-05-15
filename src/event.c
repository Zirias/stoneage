#include <SDL.h>
#include "event.h"

void
RaiseEvent(Event e)
{
    SDL_Event ev;
    SDL_UserEvent uev;

    uev.type = SDL_USEREVENT;
    uev.code = 1;
    uev.data1 = e;
    uev.data2 = 0;
    ev.type = SDL_USEREVENT;
    ev.user = uev;

    SDL_PushEvent(&ev);
}

CTOR(Event)
{
    BASECTOR(Event, Object);
    this->type = 0;
    this->sender = 0;
    this->handler = 0;
    this->data = 0;
    return this;
}

DTOR(Event)
{
    BASEDTOR(Object);
}
