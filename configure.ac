#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_PREREQ([2.6])
AC_INIT([Stoneage],
	m4_normalize(m4_include([version.inc])),
	[felix@palmen-it.de],
	[stoneage],
	[http://sekrit.de/stoneage/])

AS_IF([test -f VERSION],[. ./VERSION])

AM_INIT_AUTOMAKE([subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE
AC_CONFIG_SRCDIR([src/stoneage.c])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Misc options.
AC_ARG_ENABLE([debugging],
	AC_HELP_STRING([--enable-debugging],
	[build without optimizations and with debugging symbols]))
AS_IF([test "x$enable_debugging" = "xyes"], [CFLAGS="$CFLAGS -O0 -g3"])

AC_ARG_ENABLE([resbuild],
	AC_HELP_STRING([--enable-resbuild],
	[generate resource files even if they exist. (for maintainer usage)]))

# Checks for platforms.
AC_MSG_CHECKING([for Win32])
AS_CASE([$host],[*-mingw*],[os_win32="yes"],[os_win32="no"])
AC_MSG_RESULT([$os_win32])
AS_IF([test "x$os_win32" = "xyes"], [
       AC_DEFINE([WIN32], [1],
		 [Define to 1 if building for the Win32 platform]
		 )
       ])

# Checks for libraries.
PKG_CHECK_MODULES(SDL, [sdl >= 1.2])
tmp_CFLAGS="$CFLAGS"
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
CFLAGS="$tmp_CFLAGS"

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

# Building options

resources_exist="yes"
AS_IF([test -f bin/gfx.sar], [], [resources_exist="no"])
AS_IF([test -f bin/stoneage.ico], [], [resources_exist="no"])
AS_IF([test -f src/icondat.h], [], [resources_exist="no"])

build_resources="no"
AS_IF([test "x$enable_resbuild" = "xyes"], [build_resources="yes"])
AS_IF([test "x$resources_exist" = "xno"], [build_resources="yes"])

AS_IF([test "x$build_resources" = "xyes"],
	[AC_PATH_PROG(XCF2PNG, [xcf2png])
	AC_ARG_VAR(XCF2PNG, [path to xcf2png from xcftools])
	AS_IF([test "x$XCF2PNG" = "x"],
	      [AC_MSG_ERROR([
** xcf2png not found in your path!
** cannot build resource files.
** please make sure you have xcftools installed.
])
	      ])
	AC_PATH_PROG(CONVERT, [convert])
	AC_ARG_VAR(CONVERT, [path to convert from imagemagick])
	AS_IF([test "x$CONVERT" = "x"],
	      [AC_MSG_ERROR([
** convert not found in your path!
** cannot build resource files.
** please make sure you have imagemagick installed.
])
	      ])
	AC_PATH_PROG(XXD, [xxd])
	AC_ARG_VAR(CONVERT, [path to xxd utility from vim])
	AS_IF([test "x$CONVERT" = "x"],
	      [AC_MSG_ERROR([
** xxd not found in your path!
** cannot build resource files.
** please make sure you have vim installed.
])
	      ])
	AS_MKDIR_P([resbuild])
	AS_MKDIR_P([bin])
	])

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
AC_SUBST([stoneage_major])
AC_SUBST([stoneage_minor])
AC_SUBST([stoneage_release])
AC_DEFINE_UNQUOTED([STONEAGE_MAJOR],["$stoneage_major"],[Major version])
AC_DEFINE_UNQUOTED([STONEAGE_MINOR],["$stoneage_minor"],[Minor version])
AC_DEFINE_UNQUOTED([STONEAGE_RELEASE],["$stoneage_release"],[Release number])
AM_CONDITIONAL([WIN32], [test "x$os_win32" = "xyes"])
AM_CONDITIONAL([BUILD_RESOURCES], [test "x$build_resources" = "xyes"])
AC_CONFIG_FILES([Makefile src/stoneage.rc])
AC_OUTPUT
