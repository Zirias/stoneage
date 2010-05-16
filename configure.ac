#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.6])
AC_INIT([stoneage], [0.01], [felix@palmen-it.de])
AM_INIT_AUTOMAKE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_SRCDIR([src/stoneage.c])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
PKG_CHECK_MODULES(SDL, [sdl >= 1.2])
CFLAGS="$CFLAGS $SDL_CFLAGS"
PKG_CHECK_MODULES(SDL_image, [SDL_image >= 1.2.8],
	[check_sdl_img_128="yes"], [check_sdl_img_128="no"])
PKG_CHECK_MODULES(SDL_gfx, [SDL_gfx >= 2.0],
	[check_sdl_gfx_20="yes"], [check_sdl_gfx_20="no"])

AS_IF([test "x$check_sdl_img_128" != "xyes"],
      [AC_CHECK_HEADER([SDL_image.h],
      [],[AC_MSG_ERROR([SDL_image library could not be detected, giving up!
Please install SDL_image version 1.2.8 or newer.])],
      [#include <SDL.h>])
      ])

AS_IF([test "x$check_sdl_gfx_20" != "xyes"],
      [AC_CHECK_HEADER([SDL_rotozoom.h],
      [],[AC_MSG_ERROR([SDL_gfx library could not be detected, giving up!
Please install SDL_gfx version 2.0 or newer.])],
      [#include <SDL.h>])
      ])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strerror])

#optional stuff
AC_PATH_PROG(XCF2PNG, [xcf2png])
AC_ARG_VAR(XCF2PNG, [path to xcf2png from xcftools])

AS_IF([test "x$XCF2PNG" = x],
      [AC_MSG_WARN([
** xcf2png not found in your path!
** cannot build resource files.
** please make sure you have xcftools installed.
])],
      [AC_SUBST([RES_GFX], [gfx.sar])])

AS_IF([test "x$check_sdl_img_128" = "xno"],
      [AC_MSG_WARN([
** SDL_image >= 1.2.8 could not be found, but there seems to be some older
** version intalled. The build system will try to compile using this version.
** THIS COULD RESULT IN AN UNSUCCESSFUL BUILD!
])
      AC_DEFINE([SDL_IMG_OLD], [1],
      [Define to 1 if trying to build with an SDL_image version older than 1.2.8])
      SDL_image_LIBS=-lSDL_image])

AS_IF([test "x$check_sdl_gfx_20" = "xno"],
      [AC_MSG_WARN([
** SDL_gfx >= 2.0 could not be found, but there seems to be some older
** version installed. The build system will try to compile using this version.
** THIS COULD RESULT IN AN UNSUCCESSFUL BUILD!
])
      SDL_gfx_LIBS=-lSDL_gfx])

#output
AC_CONFIG_FILES([Makefile
                 src/Makefile
		 gfx/Makefile])
AC_OUTPUT
