#ifndef STONEAGE_EROCK_H
#define STONEAGE_EROCK_H

#include "common.h"
#include "entity.h"

/** @file erock.h
 * Includes definition of the ERock class
 */

struct Move;

/** A class representing a Rock on the game board
 */
CLASS(ERock)
{
    INHERIT(Entity);

    /** initiate falling down.
     */
    void FUNC(fall)(THIS);

    /** Attach a move to the Rock.
     * Needed to execute a "slave" move, when the rock is pushed by Willy.
     * @param m the Move
     */
    void FUNC(attachMove)(THIS, struct Move *m);
};

#endif
