#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([stoneage], [0.01], [felix@palmen-it.de])
AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([src/stoneage.c])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
PKG_CHECK_MODULES(SDL, [sdl >= 1.2])
PKG_CHECK_MODULES(SDL_image, [SDL_image >= 1.2])
PKG_CHECK_MODULES(SDL_gfx, [SDL_gfx >= 2.0])

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
      [AC_MSG_WARN([xcf2png not found in your path!])
      AC_MSG_WARN([cannot build resource files.])
      AC_MSG_NOTICE([please make sure you have xcftools installed.])],
      [AC_SUBST(RES_GFX, [gfx.sar])])

#output
AC_CONFIG_FILES([Makefile
                 src/Makefile
		 gfx/Makefile])
AC_OUTPUT
