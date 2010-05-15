#ifndef STONEAGE_COMMON_H
#define STONEAGE_COMMON_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)

#define RES_GFX QUOTEME(DATADIR) "/gfx.sar"

#include "classes.h"
#include "log.h"
#include "mm.h"
#include "object.h"

#endif

