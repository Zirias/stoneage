dist_pkgdata_DATA = gfx.sar
CLEANFILES = tempty.png tearth.png twall.png trock.png tcabbage.png twilly.png
MAINTAINERCLEANFILES = gfx.sar

if BUILD_RESOURCES
tempty.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -o tempty.png tiles.xcf empty

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

gfx.sar: stoneage.bmp tempty.png tearth.png twall.png trock.png tcabbage.png twilly.png
	$(AM_V_GEN) rm -f gfx.sar && \
	../src/samkres$(EXEEXT) gfx.sar stoneage.bmp mainicon && \
	../src/samkres$(EXEEXT) gfx.sar tempty.png tile_empty && \
	../src/samkres$(EXEEXT) gfx.sar tearth.png tile_earth && \
	../src/samkres$(EXEEXT) gfx.sar twall.png tile_wall && \
	../src/samkres$(EXEEXT) gfx.sar trock.png tile_rock && \
	../src/samkres$(EXEEXT) gfx.sar tcabbage.png tile_cabbage && \
	../src/samkres$(EXEEXT) gfx.sar twilly.png tile_willy
endif
