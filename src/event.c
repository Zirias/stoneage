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

static volatile int processingEvents = 0;
static SDL_sem *raiseLock;
static EventDelivery events[EVENT_QUEUE_SIZE];
static volatile int eventCount = 0;
static EventDelivery *head = &events[EVENT_QUEUE_SIZE];
static EventDelivery *tail = &events[EVENT_QUEUE_SIZE];

static void checkSdlEvents(void)
{
    SDL_Event *ev;

    while (1)
    {
	ev = XMALLOC(SDL_Event, 1);
	if (!SDL_PollEvent(ev))
	{
	    XFREE(ev);
	    break;
	}
	RaiseEvent(SDLEvent, (Object)0, ev);
    }
}

Event SDLEvent;

void
InitEvents(void)
{
    raiseLock = SDL_CreateSemaphore(1);
    SDLEvent = CreateEvent();
}

void
DoneEvents(void)
{
    int i;

    processingEvents = 0;

    for (i = 0; i < EVENT_QUEUE_SIZE; ++i)
    {
	events[i].active = 0;
    }

    DestroyEvent(SDLEvent);
    SDL_DestroySemaphore(raiseLock);
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

    if (eventCount >= EVENT_QUEUE_SIZE)
    {
	log_err("event queue overflow!\n");
	return;
    }
    if (head == &events[0])
    {
	head = &events[EVENT_QUEUE_SIZE];
    }
    SDL_SemWait(raiseLock);
    newDelivery = --head;
    newDelivery->active = 1;
    newDelivery->e = e;
    newDelivery->sender = sender;
    newDelivery->data = data;
    ++eventCount;
    SDL_SemPost(raiseLock);
}

void
DeliverEvents(void)
{
    EventDelivery *deliver;
    int i;

    processingEvents = 1;

    while (processingEvents)
    {
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
	SDL_Delay(10);
	checkSdlEvents();
    }
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

