#ifndef STONEAGE_MOVE_H
#define STONEAGE_MOVE_H

#include "common.h"
#include "event.h"

/** @file move.h
 * includes definition of class Move.
 */

#include <SDL.h>

struct Entity;

/** A set of predefined Trajectories.
 * These constants define types of trajectories for an entity to move
 * to another position.
 */
typedef enum
{
    TR_Linear,		/**< linear trajectory, move along a straight line. */
    TR_CircleX,		/**< circular trajectory in X-direction */
    Trajectory_count
} Trajectory;

struct Move_impl;

CLASS(Move)
{
    INHERIT(Object);

    struct Move_impl *pimpl;

    int dx;		/**< x-direction of the Move (-1, 0, 1) */ 
    int dy;		/**< y-direction of the Move (-1, 0, 1) */ 
    Event Finished;	/**< raised when the Move finished */

    /** Initialize a new Move.
     * Initializes a new Move for a given Entity with directions and trajectory.
     * @param e the Entity to move
     * @param dx x-direction of the Move (-1, 0, 1)
     * @param dy y-direction of the Move (-1, 0, 1)
     * @param t Trajectory of the Move
     * @param slave A slave move that depends on this move, or null
     */
    void FUNC(init)(THIS, struct Entity *e, int dx, int dy, Trajectory t);

    /** Sets a slave Move.
     * Sets a Move to be this Move's slave, so it doesn't run by itself but
     * lets this move control its steps. This is needed for rocks that are
     * pushed by Willy
     * @param slave the slave move
     */
    void FUNC(setSlave)(THIS, Move slave);

    /** Start a Move.
     * Starts the Move and (if set) its slave Move
     */
    void FUNC(start)(THIS);
};

/** Compute Trajectories in real pixel offsets.
 * @relates Move
 * This computes the predefined Trajectories in real pixel offsets matching
 * the given tile dimensions.
 *
 * Has to be called whenever the tile dimensions change, due to a change of
 * screen resolution.
 *
 * @param tile_width the new width of a tile
 * @param tile_height the new height of a tile
 */
extern void
computeTrajectories(int tile_width, int tile_height);

#endif
