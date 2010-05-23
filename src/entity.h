#ifndef STONEAGE_ENTITY_H
#define STONEAGE_ENTITY_H

#include <SDL.h>

#include "common.h"
#include "ehandler.h"

struct Board;
struct Move;

/** @file entity.h
 * Includes definition of the Entity class
 */

/** A class representing one entity on the game board.
 * This is the common base class for all available entities in the game
 */
CLASS(Entity)
{
    INHERIT(EHandler);
    
    struct Board *b;	/**< The game board to interact with */
    struct Move *m;	/**< Current move, 0 if not moving */
    int x;		/**< x coordinate of this entity on the board */
    int y;		/**< y coordinate of this entity on the board */

    /** Pointer for getting main tile.
     * This points to one of the Board's methods for getting a surface
     * containing the entities main tile
     */
    const SDL_Surface *FUNC(getSurface) ARG();

    /** Pointer for getting background tile.
     * For partially transparent entities, this points to one of the
     * Board's methods for getting surfaces containing the default
     * background tile this entity should blend over
     * @param x x coordinate on the board
     * @param y y coordinate on the board
     * @param buf store the surfaces here
     */
    void FUNC(getBaseSurface) ARG(int x, int y, void *buf);

    /** Initialize the entity.
     * Sets the important properties and is called by the Board class
     * upon creation of a new Entity
     * @param b the game Board
     * @param x x coordinate on the board
     * @param y y coordinate on the board
     */
    void FUNC(init) ARG(
	    struct Board *b, int x, int y);

    /** "virtual" destructor.
     * This method serves as a virtual wrapper to the destructor. It
     * should always be called instead of just using the DELETE() macro
     * directly, so derived classes get the Possibility to clean up their
     * extra data.
     */
    void FUNC(dispose) ARG();
};

#endif
