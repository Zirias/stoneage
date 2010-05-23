#ifndef STONEAGE_LEVEL_H
#define STONEAGE_LEVEL_H

#include "common.h"

/** @file level.h
 * includes definition of class Level.
 */

/** number of rows in a level
 */
#define LVL_ROWS 24

/** number of columns in a level
 */
#define LVL_COLS 32

struct Board;
struct Entity;
struct Resource;

struct Level_impl;

/** A class representing a level in the game.
 * This class is responsible for loading levels and initializing the Board
 * by creating the correspondent Entities.
 */
CLASS(Level)
{
    INHERIT(Object);

    struct Level_impl *pimpl;

    /** Create entities for current level.
     * This function takes a pointer to an array of Entities and creates
     * them there according to the current level. All previous content of
     * the entities array is overwritten.
     *
     * It is the caller's responsibility to clean up the buffer before
     * passing it to this method!
     *
     * @param b The game's Board
     * @param buffer Poiner to an array of Entities to fill
     */
    void FUNC(createEntities) ARG(struct Board *b, struct Entity **buffer);

    /** Create a random level.
     */
    void FUNC(random) ARG();

    /** Load a debugging level.
     * Loads a special debugging level defined in level.c and identified
     * by an index number.
     * @param num number of the debugging level to load
     */
    void FUNC(debug) ARG(int num);

    /** Load a level from a resource.
     * STUB
     * @param r the Resource to load the Level from
     */
    void FUNC(load) ARG(struct Resource *r);
};


#endif
