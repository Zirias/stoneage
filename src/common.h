#ifndef STONEAGE_COMMON_H
#define STONEAGE_COMMON_H

/** @file common.h
 * Includes and defines common to all source files
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)

#ifdef WIN32
#  define RES_GFX "gfx.sar"
#else
#  define RES_GFX QUOTEME(SA_PKGDATADIR) "/gfx.sar"
#  include "icon.h"
#endif

/** number of rows in a level
 */
#define LVL_ROWS 18

/** number of columns in a level
 */
#define LVL_COLS 31

#include "classes.h"
#include "log.h"
#include "mm.h"
#include "object.h"

#endif

