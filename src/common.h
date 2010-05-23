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

#include "classes.h"
#include "log.h"
#include "mm.h"
#include "object.h"

#endif

