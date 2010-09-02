#ifndef STONEAGE_EROCK_H
#define STONEAGE_EROCK_H

#include "common.h"
#include "entity.h"

/** @file erock.h
 * Includes definition of the ERock class
 */

/** A class representing a Rock on the game board
 */
CLASS(ERock)
{
    INHERIT(Entity);

    int moving;

    /** initiate falling down.
     */
    void FUNC(fall)(THIS);
};

#endif
