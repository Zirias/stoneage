#ifndef STONEAGE_EWILLY_H
#define STONEAGE_EWILLY_H

#include "common.h"
#include "entity.h"

CLASS(EWilly)
{
    INHERIT(Entity);

    int moveLock;
};

/** get current instance of Willy
 * @relates EWilly
 * Gets the last created instance of Willy. This should always be the
 * only instance in existence.
 * @return Willy :)
 */
extern EWilly
getWilly(void);

#endif
