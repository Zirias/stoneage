CLEANFILES = autoscan.log configure.scan src/stoneage.res
MAINTAINERCLEANFILES = Makefile.in configure config.guess config.sub src/config.h.in src/icondat.h

V_X2P = $(V_X2P_$(V))
V_X2P_ = $(V_X2P_$(AM_DEFAULT_VERBOSITY))
V_X2P_0 = @echo "  XCF   " $@;

V_RES = $(V_RES_$(V))
V_RES_ = $(V_RES_$(AM_DEFAULT_VERBOSITY))
V_RES_0 = @echo "  RES   " $@;

vcs-clean: maintainer-clean
	rm -fr autom4te.cache
	rm -fr doxy
	rm -f aclocal.m4 compile depcomp install-sh ltmain.sh missing

clean-local:
	rm -f resbuild/*

maintainer-clean-local:
	rm -fr resbuild bin

dist_pkgdata_DATA = bin/gfx.sar
EXTRA_DIST = bin/stoneage.ico src/icon.h src/icon.c src/icondat.h src/stoneage.rc

if BUILD_RESOURCES

resbuild/tempty.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -o resbuild/tempty.png gfx/tiles.xcf empty

resbuild/tempty_1.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -o resbuild/tempty_1.png gfx/tiles.xcf empty_1

resbuild/tempty_2a.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -o resbuild/tempty_2a.png gfx/tiles.xcf empty_2a

resbuild/tempty_2f.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -o resbuild/tempty_2f.png gfx/tiles.xcf empty_2f

resbuild/tempty_3.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -o resbuild/tempty_3.png gfx/tiles.xcf empty_3

resbuild/tempty_4.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -o resbuild/tempty_4.png gfx/tiles.xcf empty_4

resbuild/tcorner.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/tcorner.png gfx/tiles.xcf corner

resbuild/tearth.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -o resbuild/tearth.png gfx/tiles.xcf earth

resbuild/twall.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -o resbuild/twall.png gfx/tiles.xcf wall

resbuild/trock.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/trock.png gfx/tiles.xcf rock

resbuild/tcabbage.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/tcabbage.png gfx/tiles.xcf cabbage

resbuild/twilly.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/twilly.png gfx/tiles.xcf willy

bin/stoneage.ico: resbuild/twilly.png
	$(AM_V_GEN)$(CONVERT) resbuild/twilly.png \
		-trim +repage -resize 48x48\! resbuild/stoneage48.bmp && \
		$(CONVERT) resbuild/twilly.png \
		-trim +repage -resize 32x32\! resbuild/stoneage32.bmp && \
		$(CONVERT) resbuild/twilly.png \
		-trim +repage -resize 16x16\! resbuild/stoneage16.bmp && \
		$(CONVERT) resbuild/stoneage16.bmp resbuild/stoneage32.bmp \
		resbuild/stoneage48.bmp bin/stoneage.ico && \
		rm -f resbuild/stoneage48.png && \
		rm -f resbuild/stoneage32.png && \
		rm -f resbuild/stoneage16.png


resbuild/stoneage.bmp: resbuild/twilly.png
	$(AM_V_GEN)$(CONVERT) resbuild/twilly.png \
		-trim +repage -resize 32x32\! \
		-background '#999999' -flatten -fill '#00ff00' \
		-opaque '#999999' -channel rgb resbuild/stoneage.bmp

src/icondat.h: resbuild/stoneage.bmp
	$(AM_V_GEN)$(XXD) -i resbuild/stoneage.bmp src/icondat.h

bin/gfx.sar: bin/samkres$(EXEEXT) resbuild/tempty.png resbuild/tempty_1.png resbuild/tempty_2f.png resbuild/tempty_2a.png resbuild/tempty_3.png resbuild/tempty_4.png resbuild/tcorner.png resbuild/tearth.png resbuild/twall.png resbuild/trock.png resbuild/tcabbage.png resbuild/twilly.png
	$(AM_V_GEN) rm -f bin/gfx.sar && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/tempty.png tile_empty && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/tempty_1.png tile_empty_1 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/tempty_2a.png tile_empty_2a && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/tempty_2f.png tile_empty_2f && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/tempty_3.png tile_empty_3 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/tempty_4.png tile_empty_4 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/tcorner.png tile_corner && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/tearth.png tile_earth && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/twall.png tile_wall && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/trock.png tile_rock && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/tcabbage.png tile_cabbage && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/twilly.png tile_willy

endif

AM_CPPFLAGS = -DSA_PKGDATADIR='$(pkgdatadir)'

bin_PROGRAMS = bin/stoneage

bin_stoneage_LDFLAGS = $(SDL_LIBS) $(SDL_image_LIBS) $(SDL_gfx_LIBS)
bin_stoneage_SOURCES = src/common.h src/classes.h \
	src/stoneage.h src/stoneage.c \
	src/icon.h src/icon.c \
	src/log.h src/log.c \
	src/mm.h src/mm.c \
	src/object.h src/object.c \
	src/app.h src/app.c \
	src/event.h src/event.c \
	src/ehandler.h src/ehandler.c \
	src/board.h src/board.c \
	src/level.h src/level.c \
	src/entity.h src/entity.c \
	src/eearth.h src/eearth.c \
	src/ewall.h src/ewall.c \
	src/erock.h src/erock.c \
	src/ecabbage.h src/ecabbage.c \
	src/ewilly.h src/ewilly.c \
	src/resource.h src/resource.c \
	src/resfile.h src/resfile.c

src/stoneage.$(OBJEXT): src/icondat.h bin/stoneage.ico

noinst_PROGRAMS = bin/samkres

bin_samkres_SOURCES = src/common.h src/classes.h \
	src/samkres.h src/samkres.c \
	src/log.h src/log.c \
	src/mm.h src/mm.c \
	src/object.h src/object.c \
	src/app.h src/app.c \
	src/resource.h src/resource.c \
	src/resfile.h src/resfile.c

if WIN32

src/stoneage.res:
	$(V_RES) windres src/stoneage.rc -O coff -o src/stoneage.res

bin_stoneage_DEPENDENCIES = src/stoneage.res
bin_stoneage_LDADD = src/stoneage.res

endif

.PHONY: vcs-clean maintainer-clean-local clean-local

