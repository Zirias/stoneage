#ifndef STONEAGE_EVENT_H
#define STONEAGE_EVENT_H

#include "common.h"

typedef void (* EventHandler)(THIS, Object sender, void *eventData);

/** @file event.h
 * Includes definition of Event class
 */

struct Event;
/** Class representing an Event.
 */
typedef struct Event *Event;

/** Fires when an SDL Event occured.
 * The original SDL event is delivered through the eventData pointer
 */
extern Event SDLEvent;

/** Initialization of Event system.
 * Needs to be called before any Events can be used
 */
void extern InitEvents(void);

/** Cleanup of Event system.
 * Needs to be called when application is about to exit
 */
void extern DoneEvents(void);

/** Add a new handler to an event.
 * @relates Event
 * @param e the Event to handle when raised
 * @param instance a pointer to the object instance on which to invoke the handler
 * @param handler pointer to an EventHandler that is called when the Event is raised
 */
void extern AddHandler(Event e, void *instance, EventHandler handler);

/** Remove a handler from an event.
 * @relates Event;
 * @param e the Event to remove a handler from
 * @param instance a pointer to the object instance of the handler
 * @param handler a pointer to the handler method itself
 */
void extern RemoveHandler(Event e, void *instance, EventHandler handler);

/** Schedule an Event for delivery.
 * @relates Event
 * Raise an event, so all registered handlers get called as soon as the event
 * loop is running. RaiseEvent() is thread-safe, so it can be called for
 * example from SDL timer callbacks.
 * @param e the Event to raise
 * @param sender the sender of the Event
 * @param data optional Event arguments
 */
void extern RaiseEvent(Event e, Object sender, void *data);

/** Cancel a scheduled Event.
 * @relates Event
 * If the event wasn't yet delivered, it is marked as invalid to avoid
 * delivery.
 * @param e the Event to cancel
 */
void extern CancelEvent(Event e);

/** Deliver events.
 * @relates Event
 * This function is intended to be called in a main loop. It waits for events
 * and delivers them until DoneEvents() is called.
 */
void extern DoEventLoop(void);

/** Create an event instance.
 * @relates Event
 * example: Event Acivated = CreateEvent();
 * @returns an event instance
 */
Event extern CreateEvent(void);

/** Destroys an event (for use in destructors).
 * @relates Event
 * @param e the Event to destroy
 */
void extern DestroyEvent(Event e);

#endif
