#include <SDL.h>
#include <stdint.h>

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
    int active;
    Event e;
    Object sender;
    void *data;
} EventDelivery;

static EventDelivery events[EVENT_QUEUE_SIZE];
static volatile int pendingEvents = 0;
static EventDelivery * volatile head = &(events[EVENT_QUEUE_SIZE]);
static EventDelivery * volatile tail = &(events[EVENT_QUEUE_SIZE]);

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

    if (pendingEvents >= EVENT_QUEUE_SIZE)
    {
	log_err("event queue overflow!\n");
	mainApp->abort(mainApp);
    }
    if (head == &(events[0]))
    {
	head = &(events[EVENT_QUEUE_SIZE]);
    }
    newDelivery = --head;
    newDelivery->active = 1;
    newDelivery->e = e;
    newDelivery->sender = sender;
    newDelivery->data = data;
    ++pendingEvents;
}

int
DeliverEvents(void)
{
    EventDelivery *deliver;
    int i;

    if (!pendingEvents) return 0;

    for (; pendingEvents; --pendingEvents)
    {
	if (tail == &events[0])
	{
	    tail = &(events[EVENT_QUEUE_SIZE]);
	}
	deliver = --tail;
	if (deliver->active)
	{
	    for (i = 0; i < deliver->e->registeredHandlers; ++i)
	    {
		deliver->e->handlers[i].method(
			deliver->e->handlers[i].instance,
			deliver->sender, deliver->data);
	    }
	    XFREE(deliver->data);
	}
    }

    return 1;
}

void
CancelEvent(Event e)
{
    int i;

    if (!pendingEvents) return;
    for (i = 0; i < EVENT_QUEUE_SIZE; ++i)
    {
	if (events[i].e == e)
	{
	    events[i].active = 0;
	    XFREE(events[i].data);
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

