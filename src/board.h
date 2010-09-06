#ifndef STONEAGE_BOARD_H
#define STONEAGE_BOARD_H

#include <SDL.h>

#include "common.h"
#include "event.h"

struct Board_impl;

struct Entity;
struct Move;

/** @file board.h
 * Includes definition of class Board
 */

typedef struct
{
    int cancelMoves;
} MoveTickEventData;

/** This class represents the Game's Board.
 * The Board class implements all drawing functionality and some of
 * the game logic.
 */
CLASS(Board)
{
    INHERIT(Object);

    struct Board_impl *pimpl; /**< @private */

    Event MoveTick;

    /** Load and start a level.
     * STUB
     */
    void FUNC(loadLevel)(THIS, int n);

    /** Redraws the whole Board.
     * @param refresh flag indicating whether to call SDL_UpdateRect()
     */
    void FUNC(redraw)(THIS, int refresh);

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

    /** Sets paused state
     * Pauses or unpauses the movements on the board.
     * @param paused 1 for pause, 0 for resume
     */
    void FUNC(setPaused)(THIS, int paused);
};

#endif
