t%.png: tiles.xcf
	$(AM_V_GEN) $(XCF2PNG) -o $@ tiles.xcf $*

gfx.sar: tempty.png tearth.png twall.png trock.png tcabbage.png twilly.png
	$(AM_V_GEN) rm -f gfx.sar && \
	../src/samkres$(EXEEXT) gfx.sar tempty.png tile_empty && \
	../src/samkres$(EXEEXT) gfx.sar tearth.png tile_earth && \
	../src/samkres$(EXEEXT) gfx.sar twall.png tile_wall && \
	../src/samkres$(EXEEXT) gfx.sar trock.png tile_rock && \
	../src/samkres$(EXEEXT) gfx.sar tcabbage.png tile_cabbage && \
	../src/samkres$(EXEEXT) gfx.sar twilly.png tile_willy

pkgdata_DATA = $(RES_GFX)
EXTRA_DATA = gfx.sar
CLEANFILES = tempty.png tearth.png twall.png trock.png tcabbage.png twilly.png gfx.sar

