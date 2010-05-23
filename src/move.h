#ifndef STONEAGE_MOVE_H
#define STONEAGE_MOVE_H

#include "common.h"

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
    TR_Linear = 0,	/**< linear trajectory, move along a straight line. */
    TR_CircleX,		/**< circular trajectory in X-direction */
    Trajectory_count
} Trajectory;

struct Move_impl;

CLASS(Move)
{
    INHERIT(Object);

    struct Move_impl *pimpl;
    Move next;	    /**< pointer to next Move in list, used by Board. */
    Move prev;	    /**< pointer to previous Move in list, used by Board. */
    int dx;	    /**< x-direction of the Move (-1, 0, 1) */
    int dy;	    /**< y-direction of the Move (-1, 0, 1) */

    /** Initialize a new Move.
     * Initializes a new Move for a given Entity with directions and trajectory.
     * @param e the Entity to move
     * @param dx x-direction of the Move (-1, 0, 1)
     * @param dy y-direction of the Move (-1, 0, 1)
     * @param t Trajectory of the Move
     */
    void FUNC(init)(THIS, struct Entity *e, int dx, int dy, Trajectory t);

    /** Get the moved Entity.
     * @return the Entity that is moved
     */
    struct Entity *FUNC(entity)(THIS);

    /** Get the next movement step.
     * Gets the next step in pixel coordinates for the move.
     * @param x store pixel x coordinate here
     * @param y story pixel y coordinate here
     * @return 1 when this is the final step, 0 otherwise
     */
    int FUNC(step)(THIS, Sint16 *x, Sint16 *y);
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
