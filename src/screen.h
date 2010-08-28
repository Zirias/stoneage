#ifndef STONEAGE_SCREEN_H
#define STONEAGE_SCREEN_H

#include "common.h"

struct Screen_impl;

struct Resfile;

enum TileNames
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

static const char **tileNameStrings = {
    "tile_empty",
    "tile_empty_1",
    "tile_empty_2a",
    "tile_empty_2f",
    "tile_empty_3",
    "tile_empty_4",
    "tile_corner",
    "tile_earth",
    "tile_wall",
    "tile_wall_v",
    "tile_wall_h",
    "tile_rock",
    "tile_cabbage",
    "tile_willy"
};

CLASS(Screen)
{
    INHERIT(Object);

    struct Screen_impl *pimpl;

    struct Resfile *FUNC(getGraphics)(THIS);

    const SDL_Surface *FUNC(getTile)(THIS, int tile, int rotation);
};

extern Screen getScreen(void);

#endif
