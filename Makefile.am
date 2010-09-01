CLEANFILES = autoscan.log configure.scan src/stoneage.res src/config.h.in~
MAINTAINERCLEANFILES = src/icondat.h
VCSCLEANFILES = config.guess config.sub aclocal.m4 compile depcomp install-sh ltmain.sh missing version.inc configure Makefile.in src/config.h.in

V_X2P = $(V_X2P_$(V))
V_X2P_ = $(V_X2P__$(AM_DEFAULT_VERBOSITY))
V_X2P__0 = $(V_X2P_0)
V_X2P_0 = @echo "  XCF   " $@;

V_RES = $(V_RES_$(V))
V_RES_ = $(V_RES__$(AM_DEFAULT_VERBOSITY))
V_RES__0 = $(V_RES_0)
V_RES_0 = @echo "  RES   " $@;

vcs-clean: maintainer-clean
	rm -fr autom4te.cache doxy
	rm -f $(VCSCLEANFILES)

clean-local:
	rm -f resbuild/*

maintainer-clean-local:
	rm -fr resbuild bin

dist_pkgdata_DATA = bin/gfx.sar
EXTRA_DIST = VERSION bin/stoneage.ico src/icon.h src/icon.c src/icondat.h

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

resbuild/twall_v.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -o resbuild/twall_v.png gfx/tiles.xcf wall_v

resbuild/twall_h.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -o resbuild/twall_h.png gfx/tiles.xcf wall_h

resbuild/trock.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/trock.png gfx/tiles.xcf rock

resbuild/tcabbage.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/tcabbage.png gfx/tiles.xcf cabbage

resbuild/twilly.png: gfx/tiles.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/twilly.png gfx/tiles.xcf willy

resbuild/d0.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/d0.png gfx/gui.xcf digit_0

resbuild/d1.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/d1.png gfx/gui.xcf digit_1

resbuild/d2.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/d2.png gfx/gui.xcf digit_2

resbuild/d3.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/d3.png gfx/gui.xcf digit_3

resbuild/d4.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/d4.png gfx/gui.xcf digit_4

resbuild/d5.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/d5.png gfx/gui.xcf digit_5

resbuild/d6.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/d6.png gfx/gui.xcf digit_6

resbuild/d7.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/d7.png gfx/gui.xcf digit_7

resbuild/d8.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/d8.png gfx/gui.xcf digit_8

resbuild/d9.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/d9.png gfx/gui.xcf digit_9

resbuild/txpaused.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/txpaused.png gfx/gui.xcf text_paused

resbuild/txtime.png: gfx/gui.xcf
	$(V_X2P)$(XCF2PNG) -A -o resbuild/txtime.png gfx/gui.xcf text_time

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

bin/gfx.sar: bin/samkres$(EXEEXT) resbuild/tempty.png resbuild/tempty_1.png resbuild/tempty_2f.png resbuild/tempty_2a.png resbuild/tempty_3.png resbuild/tempty_4.png resbuild/tcorner.png resbuild/tearth.png resbuild/twall.png resbuild/twall_v.png resbuild/twall_h.png resbuild/trock.png resbuild/tcabbage.png resbuild/twilly.png resbuild/d0.png resbuild/d1.png resbuild/d2.png resbuild/d3.png resbuild/d4.png resbuild/d5.png resbuild/d6.png resbuild/d7.png resbuild/d8.png resbuild/d9.png resbuild/txpaused.png resbuild/txtime.png
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
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/twall_v.png tile_wall_v && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/twall_h.png tile_wall_h && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/trock.png tile_rock && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/tcabbage.png tile_cabbage && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/twilly.png tile_willy && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/d0.png digit_0 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/d1.png digit_1 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/d2.png digit_2 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/d3.png digit_3 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/d4.png digit_4 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/d5.png digit_5 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/d6.png digit_6 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/d7.png digit_7 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/d8.png digit_8 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/d9.png digit_9 && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/txpaused.png text_paused && \
	bin/samkres$(EXEEXT) bin/gfx.sar resbuild/txtime.png text_time

endif

AM_CPPFLAGS = -DSA_PKGDATADIR='$(pkgdatadir)'

bin_PROGRAMS = bin/stoneage

bin_stoneage_LDFLAGS = $(SDL_LIBS) $(SDL_image_LIBS) $(SDL_gfx_LIBS)
bin_stoneage_CFLAGS = $(SDL_CFLAGS)
bin_stoneage_SOURCES = src/common.h src/classes.h \
	src/stoneage.h src/stoneage.c \
	src/icon.h src/icon.c \
	src/log.h src/log.c \
	src/mm.h src/mm.c \
	src/object.h src/object.c \
	src/app.h src/app.c \
	src/jobqueue.h src/jobqueue.c \
	src/event.h src/event.c \
	src/ehandler.h src/ehandler.c \
	src/screen.h src/screen.c \
	src/board.h src/board.c \
	src/move.h src/move.c \
	src/level.h src/level.c \
	src/entity.h src/entity.c \
	src/eearth.h src/eearth.c \
	src/ewall.h src/ewall.c \
	src/erock.h src/erock.c \
	src/ecabbage.h src/ecabbage.c \
	src/ewilly.h src/ewilly.c \
	src/resource.h src/resource.c \
	src/resfile.h src/resfile.c

bin_stoneage_DEPENDENCIES = src/icondat.h bin/stoneage.ico
BUILT_SOURCES = src/icondat.h bin/stoneage.ico

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

bin_stoneage_DEPENDENCIES += src/stoneage.res
BUILT_SOURCES += src/stoneage.res
bin_stoneage_LDADD = src/stoneage.res

endif

.PHONY: vcs-clean maintainer-clean-local clean-local

