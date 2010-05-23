#ifndef STONEAGE_EHANDLER_H
#define STONEAGE_EHANDLER_H

#include "common.h"

struct Event;

/** @file ehandler.h
 * Includes definition of EHandler class
 */

/** "Interface" for Event handlers.
 * This Class defines a single method "handleEvent", but does not implement
 * it. Classes that need the ability to handle Events must inherit this class.
 */
CLASS(EHandler)
{
    INHERIT(Object);

    /** Handle an Event.
     * @param e the event to handle
     */
    void FUNC(handleEvent)(THIS, struct Event *e);
};

#endif
