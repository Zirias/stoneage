#ifndef STONEAGE_EVENT_H
#define STONEAGE_EVENT_H

#include "common.h"

struct EHandler;

/** @file event.h
 * Includes definition of Event class
 */

/** A type of an Event.
 * This enumerates all possible event types
 */
enum EventType
{
    SAEV_MoveTick,
    SAEV_MoveFinished,
};

/** Class representing an Event.
 */
CLASS(Event)
{
    INHERIT(Object);

    enum EventType type;	/**< the event type */
    Object sender;		/**< who sent the event */
    struct EHandler *handler;	/**< who should handle the event */
    void *data;			/**< optional extra data */
};

/** Schedule an Event for delivery.
 * @relates Event
 * @param e the Event to deliver
 */
void extern RaiseEvent(Event e);

/** Deliver an Event.
 * @relates Event
 * The event is checked for validity and the receiver object is checked
 * whether it actually provides the handleEvent() implementation. If the
 * event can't be delivered, it is deleted and forgotten.
 * @param e the Event to deliver
 */
void extern DeliverEvent(Event e);

/** Cancel a scheduled Event.
 * @relates Event
 * If the event wasn't yet delivered, it is marked as invalid to avoid
 * delivery.
 * @param e the Event to deliver
 */
void extern CancelEvent(Event e);

/** Cancel all Events addressed to a given receiver.
 * @relates Event
 * All pending events for the given receiver are marked invalid.
 * @param h the receiver whose events should be canceled
 */
void extern CancelEventsFor(struct EHandler *h);

#endif
