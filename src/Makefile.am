
bin_PROGRAMS = stoneage
stoneage_SOURCES = stoneage.c log.c mm.c object.c app.c event.c ehandler.c board.c entity.c eearth.c ewall.c erock.c ecabbage.c ewilly.c resource.c resfile.c
stoneage_CPPFLAGS = -DDATADIR='$(pkgdatadir)'
stoneage_CFLAGS = $(SDL_CFLAGS)
stoneage_LDFLAGS = $(SDL_LIBS) $(SDL_image_LIBS) $(SDL_gfx_LIBS)

noinst_PROGRAMS = samkres
samkres_SOURCES = samkres.c log.c mm.c object.c app.c resource.c resfile.c

