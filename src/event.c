#include <SDL.h>
#include <stdint.h>

#include "event.h"
#include "ehandler.h"
#include "app.h"

typedef struct
{
    int valid;
    Event e;
    EHandler h;
} EventRecord;

static EventRecord EventRegistry[64];

static void
registerEvent(Event e)
{
    EventRecord *r;
    EventRecord *end = &EventRegistry[64];
    for(r = &EventRegistry[0]; r != end; ++r)
    {
	if (!r->valid)
	{
	    r->valid = 1;
	    r->e = e;
	    r->h = e->handler;
	    return;
	}
    }
    log_err("event queue overrun!\n");
    DELETE(Event, e);
    mainApp->abort(mainApp);
}

static int
confirmEvent(Event e)
{
    EventRecord *r;
    EventRecord *end = &EventRegistry[64];
    for(r = &EventRegistry[0]; r != end; ++r)
    {
	if (!r->valid) continue;
	if (r->e == e)
	{
	    r->valid = 0;
	    return 1;
	}
    }
    return 0;
}

void
DeliverEvent(Event e)
{
    if (!e) return;
    if (!confirmEvent(e))
    {
	DELETE(Event, e);
	return;
    }

    if (!e->handler || !e->handler->handleEvent)
    {
	DELETE(Event, e);
	return;
    }
    e->handler->handleEvent(e->handler, e);
}

void
CancelEvent(Event e)
{
    EventRecord *r;
    EventRecord *end = &EventRegistry[64];
    for(r = &EventRegistry[0]; r != end; ++r)
    {
	if (!r->valid) continue;
	if (r->e == e)
	{
	    r->valid = 0;
	    return;
	}
    }
}

void
CancelEventsFor(EHandler h)
{
    EventRecord *r;
    EventRecord *end = &EventRegistry[64];
    for(r = &EventRegistry[0]; r != end; ++r)
    {
	if (!r->valid) continue;
	if (r->h == h)
	{
	    r->valid = 0;
	}
    }
}

void
RaiseEvent(Event e)
{
    SDL_Event ev;
    SDL_UserEvent uev;

    registerEvent(e);

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
    XFREE(this->data);
    BASEDTOR(Object);
}
