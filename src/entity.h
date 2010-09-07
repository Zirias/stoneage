#ifndef STONEAGE_ENTITY_H
#define STONEAGE_ENTITY_H

#include <SDL.h>

#include "common.h"
#include "event.h"

/** @file entity.h
 * Includes definition of the Entity class
 */

struct Move;

typedef enum
{
    BG_None,
    BG_Empty,
    BG_Earth
} Background;

typedef struct
{
    struct Move *m;
} MoveStartingEventData;

typedef struct
{
    int from_x;
    int from_y;
    int to_x;
    int to_y;
} PositionChangedEventData;

/** A class representing one entity on the game board.
 * This is the common base class for all available entities in the game
 */
CLASS(Entity)
{
    INHERIT(Object);
    
    struct Board *b;	/**< The game board to interact with */
    int moving;		/**< 1 when moving, 0 otherwise */
    int x;		/**< x coordinate of this entity on the board */
    int y;		/**< y coordinate of this entity on the board */
    Background bg;	/**< type of the background for this entity */

    Event MoveStarting;
    Event PositionChanged;

    /** Draw the entity on the board.
     * Draws the entity on the board on its current position
     * @param refresh whether to refresh the drawing surface using SDL_UpdateRect()
     */
    void FUNC(draw)(THIS, int refresh);

    /** Get the foreground tile for the entity.
     * Get an SDL Surface containing the foreground tile of this entity or
     * null if this entity has no foreground tile
     */
    const SDL_Surface *FUNC(getTile)(THIS);

    /** Draw the entities background on the board.
     * Draws only the background of the entity on its current position on
     * the board, but not the entity itself (needed for examples for moving
     * entities)
     * @param refresh whether to refresh the drawing surface using SDL_UpdateRect()
     */
    void FUNC(drawBackground)(THIS, int refresh);

    /** Initialize the entity.
     * Sets the important properties and is called by the Board class
     * upon creation of a new Entity
     * @param b the game Board
     * @param x x coordinate on the board
     * @param y y coordinate on the board
     */
    void FUNC(init)(THIS, struct Board *b, int x, int y);

    /** "virtual" destructor.
     * This method serves as a virtual wrapper to the destructor. It
     * should always be called instead of just using the DELETE() macro
     * directly, so derived classes get the Possibility to clean up their
     * extra data.
     */
    void FUNC(dispose)(THIS);
};

#endif
