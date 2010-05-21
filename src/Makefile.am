MAINTAINERCLEANFILES = Makefile.in config.h.in

AM_CPPFLAGS = -DDATADIR='$(pkgdatadir)'

bin_PROGRAMS = stoneage

stoneage_LDFLAGS = $(SDL_LIBS) $(SDL_image_LIBS) $(SDL_gfx_LIBS)
stoneage_SOURCES = common.h classes.h \
	stoneage.h stoneage.c \
	log.h log.c \
	mm.h mm.c \
	object.h object.c \
	app.h app.c \
	event.h event.c \
	ehandler.h ehandler.c \
	board.h board.c \
	level.h level.c \
	entity.h entity.c \
	eearth.h eearth.c \
	ewall.h ewall.c \
	erock.h erock.c \
	ecabbage.h ecabbage.c \
	ewilly.h ewilly.c \
	resource.h resource.c \
	resfile.h resfile.c

noinst_PROGRAMS = samkres

samkres_SOURCES = common.h classes.h \
	samkres.h samkres.c \
	log.h log.c \
	mm.h mm.c \
	object.h object.c \
	app.h app.c \
	resource.h resource.c \
	resfile.h resfile.c
