#ifndef STONEAGE_BOARD_H
#define STONEAGE_BOARD_H

#include <SDL.h>

#include "common.h"
#include "ehandler.h"

struct Board_impl;

struct Entity;
struct Move;

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

    /** Set geometry information of the board.
     * This must be called when the size of a tile is about to change, e.g.
     * due to a new screen resolution. The drawing rectangle for SDL drawing
     * functions and the individual points of movement trajectories are
     * calculated when this is called.
     * @param width new width of a tile
     * @param height new height of a tile
     * @param off_x x offset for the whole board
     * @param off_y y offset for the whole board
     */
    void FUNC(setGeometry)(THIS, int width, int height,
	    int off_x, int off_y);

    /** Load and start a level.
     * STUB
     */
    void FUNC(loadLevel)(THIS, int n);

    /** Redraws the whole Board.
     */
    void FUNC(redraw)(THIS);

    /** Draw tile at the given position.
     * The given position is checked for a game entity. If one is found,
     * tiles are drawn as given by the entity's getBaseSurface() and
     * getSurface() function pointers, otherwise the "empty" tile is used.
     * @param x x-coordinate on the board
     * @param y y-coordinate on the board
     * @param refresh flag indicating whether to call SDL_UpdateRects()
     */
    void FUNC(draw)(THIS, int x, int y, int refresh);

    /** Get entity at given position.
     * @param x x-coordinate on the board
     * @param y y-coordinate on the board
     * @param e store the Entity (or 0 if the given position is empty) here
     * @return non-zero if the coordinates are invalid
     */
    int FUNC(entity)(THIS, int x, int y, struct Entity **e);

    /** Start a moving operation.
     * Start movement described by a given Move object
     * @param m a Move object describing the motion.
     */
    void FUNC(startMove)(THIS, struct Move *m);

    /** Get SDL Surfaces with "Empty" Tiles suitable for the given position.
     * @return SDL Surface
     */
    void FUNC(getEmptyBackground)(THIS, int x, int y, void *buf);

    /** Get SDL Surfaces with "Earth" Tiles suitable for the given position.
     * @return SDL Surface
     */
    void FUNC(getEarthBackground)(THIS, int x, int y, void *buf);

    /** Get SDL Surface with "Earth" Tile.
     * @return SDL Surface
     */
    const SDL_Surface *FUNC(getEarthTile)(THIS);

    /** Get SDL Surface with "Wall" Tile.
     * @return SDL Surface
     */
    const SDL_Surface *FUNC(getWallTile)(THIS);

    /** Get SDL Surface with "Rock" Tile.
     * @return SDL Surface
     */
    const SDL_Surface *FUNC(getRockTile)(THIS);

    /** Get SDL Surface with "Cabbage" Tile.
     * @return SDL Surface
     */
    const SDL_Surface *FUNC(getCabbageTile)(THIS);

    /** Get SDL Surface with "Willy" Tile.
     * @return SDL Surface
     */
    const SDL_Surface *FUNC(getWillyTile)(THIS);
};

#endif
