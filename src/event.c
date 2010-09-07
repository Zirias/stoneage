#include <SDL.h>

#include "event.h"
#include "app.h"

struct EventHandlerEntry;
typedef struct EventHandlerEntry EventHandlerEntry;

struct EventHandlerEntry
{
    void *instance;
    EventHandler method;
    EventHandlerEntry *next;
};

struct Event
{
    EventHandlerEntry *handlers;
};

struct EventDelivery;
typedef struct EventDelivery EventDelivery;

struct EventDelivery
{
    volatile int active;
    Event e;
    Object sender;
    void *data;
    EventDelivery *next;
    EventDelivery *prev;
};

static volatile int raiseLock = 1;
static EventDelivery events = { -1, 0, 0, 0, 0, 0 };

static SDL_Event sdlEv;

Event SDLEvent;
static EventDelivery SDLEventDelivery;

void
InitEvents(void)
{
    SDLEvent = CreateEvent();
    sdlEv.type = SDL_USEREVENT;
    sdlEv.user.type = SDL_USEREVENT;
    sdlEv.user.code = 0;
    SDLEventDelivery.active = 1;
    SDLEventDelivery.e = SDLEvent;
    SDLEventDelivery.sender = 0;
    SDLEventDelivery.data = 0;
}

void
DoneEvents(void)
{
    EventDelivery *deliver;

    deliver = events.next;
    while (deliver)
    {
	deliver->active = 0;
	deliver = deliver->next;
    }
    sdlEv.user.code = -1;
    SDL_PushEvent(&sdlEv);

    DestroyEvent(SDLEvent);
}

void
AddHandler(Event e, void *instance, EventHandler handler)
{
    EventHandlerEntry *current;
    EventHandlerEntry *entry;
    
    entry = XMALLOC(EventHandlerEntry, 1);
    entry->instance = instance;
    entry->method = handler;
    entry->next = 0;

    if (e->handlers)
    {
	current = e->handlers;
	while (current->next) current = current->next;
	current->next = entry;
    }
    else
    {
	e->handlers = entry;
    }
}

void
RemoveHandler(Event e, void *instance, EventHandler handler)
{
    EventHandlerEntry *entry;
    EventHandlerEntry *previous;

    for (entry = e->handlers, previous = 0;
	    entry;
	    previous = entry, entry = entry->next)
    {
	if (entry->instance != instance || entry->method != handler) continue;
	if (previous)
	    previous->next = entry->next;
	else
	    e->handlers = entry->next;
	XFREE(entry);
	break;
    }
}

void
RaiseEvent(Event e, Object sender, void *data)
{
    EventDelivery *newDelivery;

    if (!e->handlers) return;

    newDelivery = XMALLOC(EventDelivery, 1);
    newDelivery->active = 1;
    newDelivery->e = e;
    newDelivery->sender = sender;
    newDelivery->data = data;
    newDelivery->next = 0;
    /* begin critical section */
    while (--raiseLock) { ++raiseLock; SDL_Delay(10); }
    newDelivery->prev = events.prev;
    if (events.next)
	events.prev->next = newDelivery;
    else
	events.next = newDelivery;
    events.prev = newDelivery;
    ++raiseLock;
    /* end critical section */
    sdlEv.user.data1 = newDelivery;
    SDL_PushEvent(&sdlEv);
}

void
DoEventLoop(void)
{
    EventDelivery *deliver;
    EventHandlerEntry *entry;
    SDL_Event ev;
    int isSdl;

    while (1)
    {
	SDL_WaitEvent(&ev);
	if (ev.type == SDL_USEREVENT)
	{
	    if (ev.user.code < 0) break;
	    deliver = ev.user.data1;
	    isSdl = 0;
	}
	else
	{
	    SDLEventDelivery.data = &ev;
	    deliver = &SDLEventDelivery;
	    isSdl = 1;
	}
	for (entry = deliver->e->handlers;
		entry;
		entry = entry->next)
	{
	    entry->method(entry->instance, deliver->sender, deliver->data);
	    if (!deliver->active) break;
	}
	if (isSdl) continue;
	XFREE(deliver->data);
	/* begin critical section */
	while (--raiseLock) { ++raiseLock; SDL_Delay(10); }
	if (deliver->next)
	{
	    deliver->next->prev = deliver->prev;
	}
	else
	{
	    events.prev = deliver->prev;
	}
	if (deliver->prev)
	{
	    deliver->prev->next = deliver->next;
	}
	else
	{
	    events.next = deliver->next;
	}
	++raiseLock;
	/* end critical section */
	XFREE(deliver);
    }
}

void
CancelEvent(Event e)
{
    EventDelivery *deliver;

    deliver = events.next;
    while (deliver)
    {
	if (deliver->e == e) deliver->active = 0;
	deliver = deliver->next;
    }
}

Event
CreateEvent(void)
{
    Event e = XMALLOC(struct Event, 1);
    e->handlers = 0;
    return e;
}

void
DestroyEvent(Event e)
{
    EventHandlerEntry *entry;
    EventHandlerEntry *next;

    CancelEvent(e);
    for (entry = 0, next = e->handlers;
	    next;
	    entry = next, next = entry->next)
    {
	XFREE(entry);
    }
    if (entry) XFREE(entry);
    XFREE(e);
}

