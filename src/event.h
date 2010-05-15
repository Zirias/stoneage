#ifndef STONEAGE_EVENT_H
#define STONEAGE_EVENT_H

#include "common.h"

struct EHandler;

enum EventType
{
    SAEV_MoveTick,
    SAEV_MoveFinished,
};

CLASS(Event)
{
    INHERIT(Object);

    enum EventType type;
    Object sender;
    struct EHandler *handler;
    void *data;
};

void extern RaiseEvent(Event e);
void extern DeliverEvent(Event e);
void extern CancelEvent(Event e);
void extern CancelEventsFor(struct EHandler *h);

#endif
