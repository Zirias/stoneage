#ifndef STONEAGE_LEVEL_H
#define STONEAGE_LEVEL_H

#include "common.h"

/** @file level.h
 * includes definition of class Level.
 */

/** number of rows in a level
 */
#define LVL_ROWS 18

/** number of columns in a level
 */
#define LVL_COLS 31

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
    void FUNC(createEntities)(THIS, struct Board *b, struct Entity **buffer);

    /** Create a random level.
     */
    void FUNC(random)(THIS);

    /** Load a builtin level.
     * Loads one of the hardcoded levels by index number.
     * @param num number of the level to load
     */
    void FUNC(builtin)(THIS, int num);

    /** Load a level from a resource.
     * STUB
     * @param r the Resource to load the Level from
     */
    void FUNC(load)(THIS, struct Resource *r);
};


#endif
