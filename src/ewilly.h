#ifndef STONEAGE_EWILLY_H
#define STONEAGE_EWILLY_H

#include "common.h"
#include "entity.h"

CLASS(EWilly)
{
    INHERIT(Entity);

    int moveLock;
    int alive;

    /** Move willy.
     * Moves willy in the given direction.
     * @param dx x-direction (-1, 0, 1)
     * @param dy y-direction (-1, 0, 1)
     */
    void FUNC(move)(THIS, int dx, int dy);
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
