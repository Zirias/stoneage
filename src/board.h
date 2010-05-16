#ifndef STONEAGE_BOARD_H
#define STONEAGE_BOARD_H

#include <SDL.h>

#include "common.h"
#include "ehandler.h"

struct Board_impl;

struct Entity;

/** @file board.h
 * Includes definition of class Board
 */

/** This class represents the Game's Board.
 * The Board class implements all drawing functionality and some of
 * the game logic.
 */
CLASS(Board)
{
    INHERIT(EHandler);

    struct Board_impl *pimpl; /**< @private */

    /** Initialize the display of the board.
     * This method must be called whenever the video-mode changes.
     * It calulates tile sizes.
     */
    void FUNC(initVideo) ARG();

    /** Load and start a level.
     * STUB
     */
    void FUNC(loadLevel) ARG();

    /** Redraws the whole Board.
     */
    void FUNC(redraw) ARG();

    /** Draw tile at the given position.
     * The given position is checked for a game entity. If one is found,
     * tiles are drawn as given by the entity's getBaseSurface() and
     * getSurface() function pointers, otherwise the "empty" tile is used.
     * @param x x-coordinate on the board
     * @param y y-coordinate on the board
     * @param refresh flag indicating whether to call SDL_UpdateRects()
     */
    void FUNC(draw) ARG(int x, int y, int refresh);

    /** Check for entity at a given position.
     * Checks the given position for an entity.
     * @param x x-coordinate on the board
     * @param y y-coordinate on the board
     * @return 1 if the given position is found empty, 0 otherwise
     */
    int FUNC(isEmpty) ARG(int x, int y);

    /** Start a moving operation.
     * Start movement of a given entity on the board in dir_x, dir_y
     * direction.
     * @param e the Entity to move
     * @param dir_x x direction, valid values -1, 0 or 1
     * @param dir_y y direction, valid values -1, 0 or 1
     */
    void FUNC(startMove) ARG(
	    struct Entity *e, int dir_x, int dir_y);

    /** Get SDL Surface with "Empty" Tile.
     * @return SDL Surface
     */
    const SDL_Surface *FUNC(getEmptyTile) ARG();

    /** Get SDL Surface with "Earth" Tile.
     * @return SDL Surface
     */
    const SDL_Surface *FUNC(getEarthTile) ARG();

    /** Get SDL Surface with "Wall" Tile.
     * @return SDL Surface
     */
    const SDL_Surface *FUNC(getWallTile) ARG();

    /** Get SDL Surface with "Rock" Tile.
     * @return SDL Surface
     */
    const SDL_Surface *FUNC(getRockTile) ARG();

    /** Get SDL Surface with "Cabbage" Tile.
     * @return SDL Surface
     */
    const SDL_Surface *FUNC(getCabbageTile) ARG();

    /** Get SDL Surface with "Willy" Tile.
     * @return SDL Surface
     */
    const SDL_Surface *FUNC(getWillyTile) ARG();
};

#endif
