#ifndef STONEAGE_SCREEN_H
#define STONEAGE_SCREEN_H

#include <SDL.h>

#include "common.h"

struct Screen_impl;

struct Resfile;
struct Board;

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

    /** Get a surface for a particular tile in a given rotation.
     * This function returns a SDL-Surface of a tile in a given rotation.
     * Loading/rotating of the tile is handled automatically if necessary.
     * @param tile which tile to return
     * @param rotation 0 - 3 for 0°, 90°, 180° or 270°
     * @return SDL-Surface of the requested tile
     */
    const SDL_Surface *FUNC(getTile)(THIS, enum TileName tile, int rotation);

    /** Get the board on the screen.
     * Returns the board that is displayed on the screen.
     * @return the board
     */
    struct Board *FUNC(getBoard)(THIS);

    /** Get real pixel position for a given coordinate.
     * This function gives the upper left edge of a board coordinate in
     * real pixels.
     * @param x x-coordinate on the board
     * @param y y-coordinate on the board
     * @param px x component of pixel position
     * @param py y component of pixel position
     * @return non-zero on error (coordinate out of board range)
     */
    int FUNC(coordinatesToPixel)(THIS, int x, int y, Sint16 *px, Sint16 *py);
};

extern Screen getScreen(void);

#endif
