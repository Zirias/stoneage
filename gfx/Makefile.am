CLEANFILES = tempty.png tempty_1.png tempty_2a.png tempty_2f.png tempty_3.png tempty_4.png tearth.png twall.png trock.png tcabbage.png twilly.png
MAINTAINERCLEANFILES = gfx.sar Makefile.in

dist_pkgdata_DATA = gfx.sar

if BUILD_RESOURCES
tempty.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -o tempty.png tiles.xcf empty

tempty_1.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -o tempty_1.png tiles.xcf empty_1

tempty_2a.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -o tempty_2a.png tiles.xcf empty_2a

tempty_2f.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -o tempty_2f.png tiles.xcf empty_2f

tempty_3.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -o tempty_3.png tiles.xcf empty_3

tempty_4.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -o tempty_4.png tiles.xcf empty_4

tearth.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -o tearth.png tiles.xcf earth

twall.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -o twall.png tiles.xcf wall

trock.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -A -o trock.png tiles.xcf rock

tcabbage.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -A -o tcabbage.png tiles.xcf cabbage

twilly.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -A -o twilly.png tiles.xcf willy

gfx.sar: stoneage.bmp tempty.png tempty_1.png tempty_2f.png tempty_2a.png tempty_3.png tempty_4.png tearth.png twall.png trock.png tcabbage.png twilly.png
	$(AM_V_GEN) rm -f gfx.sar && \
	../src/samkres$(EXEEXT) gfx.sar stoneage.bmp mainicon && \
	../src/samkres$(EXEEXT) gfx.sar tempty.png tile_empty && \
	../src/samkres$(EXEEXT) gfx.sar tempty_1.png tile_empty_1 && \
	../src/samkres$(EXEEXT) gfx.sar tempty_2a.png tile_empty_2a && \
	../src/samkres$(EXEEXT) gfx.sar tempty_2f.png tile_empty_2f && \
	../src/samkres$(EXEEXT) gfx.sar tempty_3.png tile_empty_3 && \
	../src/samkres$(EXEEXT) gfx.sar tempty_4.png tile_empty_4 && \
	../src/samkres$(EXEEXT) gfx.sar tearth.png tile_earth && \
	../src/samkres$(EXEEXT) gfx.sar twall.png tile_wall && \
	../src/samkres$(EXEEXT) gfx.sar trock.png tile_rock && \
	../src/samkres$(EXEEXT) gfx.sar tcabbage.png tile_cabbage && \
	../src/samkres$(EXEEXT) gfx.sar twilly.png tile_willy
endif
