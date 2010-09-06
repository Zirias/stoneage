#ifndef STONEAGE_SCREEN_H
#define STONEAGE_SCREEN_H

#include <SDL.h>

#include "common.h"

struct Screen_impl;

struct Resfile;
struct Board;

/** @file screen.h
 * Includes definition of class Screen
 */

/** The different types of tile graphics
 * This is a collection of symbolic names for the tile graphics used in the
 * game
 */
enum TileName
{
    SATN_Empty,
    SATN_Empty_1,
    SATN_Empty_2a,
    SATN_Empty_2f,
    SATN_Empty_3,
    SATN_Empty_4,
    SATN_Corner,
    SATN_Earth,
    SATN_Wall,
    SATN_Wall_v,
    SATN_Wall_h,
    SATN_Rock,
    SATN_Cabbage,
    SATN_Willy,
    SATN_NumberOfTiles
};

/** The different types of text graphics
 * This is a collection of symbolic names for the text graphics used in the
 * game
 */
enum TextName
{
    SATX_Paused,
    SATX_Time,
    SATX_NumberOfTexts
};

/** This class represents the whole SDL screen.
 * The screen class includes primitives for loading, scaling and rotating
 * tile graphics as well as managing the position of the Board and the
 * size of the tiles.
 */
CLASS(Screen)
{
    INHERIT(Object);

    struct Screen_impl *pimpl;

    /** Initialize the display of the board.
     * This method must be called whenever the video-mode changes.
     * It calulates tile sizes.
     */
    void FUNC(initVideo)(THIS);

    /** Starts the game.
     * Starts the game by loading the first level.
     */
    void FUNC(startGame)(THIS);

    /** Counts time down.
     * Counts time down by one second
     */
    void FUNC(timeStep)(THIS);

    /** Get a surface for a particular tile in a given rotation.
     * This function returns a SDL-Surface of a tile in a given rotation.
     * Loading/rotating of the tile is handled automatically if necessary.
     * @param tile which tile to return
     * @param rotation 0 - 3 for 0°, 90°, 180° or 270°
     * @return SDL-Surface of the requested tile
     */
    const SDL_Surface *FUNC(getTile)(THIS, enum TileName tile, int rotation);

    /** Get a surface containing a particular text
     * This function returns a SDL-surface for a given text, it is scaled
     * automatically.
     * @param text which text to return
     * @return SDL-Surface of the requested text.
     */
    const SDL_Surface *FUNC(getText)(THIS, enum TextName text);

    /** Get the board on the screen.
     * Returns the board that is displayed on the screen.
     * @return the board
     */
    struct Board *FUNC(getBoard)(THIS);

    /** Get real pixel coordinates for given board coordinates.
     * This function transforms given board coordinates to an SDL rectangle of
     * real pixel coordinates suitable for drawing tiles to the board.
     * @param x x-coordinate on the board
     * @param y y-coordinate on the board
     * @param px x component of pixel position
     * @param py y component of pixel position
     * @return non-zero on error (coordinate out of board range)
     */
    int FUNC(coordinatesToRect)(THIS, int x, int y, int w, int h, SDL_Rect *rect);
};

extern Screen getScreen(void);

#endif
