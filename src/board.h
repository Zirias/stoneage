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

    /** Initialize the display of the board.
     * This method must be called whenever the video-mode changes.
     * It calulates tile sizes.
     */
    void FUNC(initVideo)(THIS);

    /** Load and start a level.
     * STUB
     */
    void FUNC(loadLevel)(THIS);

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

    /** Get real pixel position for a given coordinate.
     * This function gives the upper left edge of a board coordinate in
     * real pixels.
     * @param x x-coordinate on the board
     * @param y y-coordinate on the board
     * @param px x component of pixel position
     * @param py y component of pixel position
     * @return non-zero on error (coordinate out of board range)
     */
    int FUNC(coordinatesToPixel)(THIS, int x, int y, int *px, int *py);

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

    /** Get SDL Surface with "Empty" Tile suitable for the given position.
     * @return SDL Surface
     */
    void FUNC(getEmptyTile)(THIS, int x, int y, void *buf);

    /** Get SDL Surface with "Earth" Tile suitable for the given position.
     * @return SDL Surface
     */
    void FUNC(getEarthBaseTile)(THIS, int x, int y, void *buf);

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
