#include <SDL.h>
#include <SDL_thread.h>

#include "event.h"
#include "app.h"

#define EVENT_QUEUE_SIZE 64
#define MAX_EVENT_HANDLERS 16

struct Event
{
    int registeredHandlers;
    struct
    {
	void *instance;
	EventHandler method;
    } handlers[MAX_EVENT_HANDLERS];
};

typedef struct
{
    volatile int active;
    Event e;
    Object sender;
    void *data;
} EventDelivery;

static SDL_sem *pendingEvents;
static SDL_sem *raiseLock;
static EventDelivery events[EVENT_QUEUE_SIZE];
static int eventCount = 0;
static EventDelivery *head = &events[EVENT_QUEUE_SIZE];
static EventDelivery *tail = &events[EVENT_QUEUE_SIZE];

static SDL_Thread *SDLEventListener;

static int SDLEventListenerMain(void *data)
{
    SDL_Event *ev;

    while (1)
    {
	ev = XMALLOC(SDL_Event, 1);
	SDL_WaitEvent(ev);
	if (ev->type == SDL_USEREVENT)
	{
	    XFREE(ev);
	    break;
	}
	RaiseEvent(SDLEvent, (Object)0, ev);
    }
    return 0;
}

Event SDLEvent;

void
InitEvents(void)
{
    pendingEvents = SDL_CreateSemaphore(0);
    raiseLock = SDL_CreateSemaphore(1);
    SDLEvent = CreateEvent();
    SDLEventListener = SDL_CreateThread(&SDLEventListenerMain, 0);
}

void
DoneEvents(void)
{
    int i;
    SDL_Event e;

    e.type = SDL_USEREVENT;
    SDL_PushEvent(&e);
    SDL_WaitThread(SDLEventListener, 0);

    for (i = 0; i < EVENT_QUEUE_SIZE; ++i)
    {
	events[i].active = 0;
    }
    DeliverEvents(0);

    DestroyEvent(SDLEvent);
    SDL_DestroySemaphore(raiseLock);
    SDL_DestroySemaphore(pendingEvents);
}

void
AddHandler(Event e, void *instance, EventHandler handler)
{
    if (e->registeredHandlers == MAX_EVENT_HANDLERS)
    {
	log_err("event handler overflow!\n");
	mainApp->abort(mainApp);
    }
    e->handlers[e->registeredHandlers].instance = instance;
    e->handlers[e->registeredHandlers++].method = handler;
}

void
RaiseEvent(Event e, Object sender, void *data)
{
    EventDelivery *newDelivery;

    SDL_SemWait(raiseLock);
    if (eventCount >= EVENT_QUEUE_SIZE)
    {
	log_err("event queue overflow!\n");
	mainApp->abort(mainApp);
    }
    if (head == &events[0])
    {
	head = &events[EVENT_QUEUE_SIZE];
    }
    newDelivery = --head;
    newDelivery->active = 1;
    newDelivery->e = e;
    newDelivery->sender = sender;
    newDelivery->data = data;
    ++eventCount;
    if (SDL_SemValue(pendingEvents)) SDL_SemPost(pendingEvents);
    SDL_SemPost(raiseLock);
}

int
DeliverEvents(wait)
{
    EventDelivery *deliver;
    int i;

    if (wait)
    {
	SDL_SemWait(pendingEvents);
    }
    else
    {
	if (!SDL_SemTryWait(pendingEvents)) return 0;
    }

    while (eventCount)
    {
	if (tail == &events[0])
	{
	    tail = &events[EVENT_QUEUE_SIZE];
	}
	deliver = --tail;
	for (i = 0;
		deliver->active && i < deliver->e->registeredHandlers;
		++i )
	{
	    deliver->e->handlers[i].method(
		    deliver->e->handlers[i].instance,
		    deliver->sender, deliver->data);
	}
	XFREE(deliver->data);
	--eventCount;
    }

    return 1;
}

void
CancelEvent(Event e)
{
    int i;

    if (!eventCount) return;
    for (i = 0; i < EVENT_QUEUE_SIZE; ++i)
    {
	if (events[i].e == e)
	{
	    events[i].active = 0;
	}
    }
}

Event
CreateEvent(void)
{
    Event e = XMALLOC(struct Event, 1);
    e->registeredHandlers = 0;
    return e;
}

void
DestroyEvent(Event e)
{
    CancelEvent(e);
    XFREE(e);
}

