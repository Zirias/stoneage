#include "stoneage.h"
#include "board.h"
#include "resfile.h"
#include "resource.h"
#include "event.h"

Uint32
createTickerEvent(Uint32 interval, void *param)
{
    Stoneage this = CAST(param, Stoneage);

    SDL_Event e;
    SDL_UserEvent ue;
    ue.type = SDL_USEREVENT;
    ue.code = 0;
    ue.data1 = 0;
    ue.data2 = 0;
    e.type = SDL_USEREVENT;
    e.user = ue;
    SDL_PushEvent(&e);
    return interval;
}

static void
setupVideo(Stoneage this)
{
    this->screen = SDL_SetVideoMode(
	    this->res_x, this->res_y, this->bpp, this->modeflags);
    if (!this->screen)
    {
	log_err("Unable to set video mode: %s\n", SDL_GetError());
	((App)this)->abort(this);
    }
}

static void
toggleFullscreen(Stoneage this)
{
    this->modeflags ^= (SDL_HWSURFACE|SDL_FULLSCREEN);
    setupVideo(this);
    this->board->initVideo(this->board);
}

static void
handleKeyboardEvent(Stoneage this, SDL_KeyboardEvent *e)
{
    if (e->state == SDL_PRESSED)
    {
	if (e->keysym.mod & KMOD_ALT)
	{
	    switch (e->keysym.sym)
	    {
		case SDLK_RETURN:
		    toggleFullscreen(this);
		    break;
	    }
	}
	else if (e->keysym.mod & KMOD_CTRL)
	{
	    switch (e->keysym.sym)
	    {
		case SDLK_n:
		    this->board->loadLevel(this->board);
		    break;
	    }
	}
    }
}

static void
handleTick(Stoneage this)
{
    
}

static int
m_run ARG(int argc, char **argv)
{
    METHOD(Stoneage);

    SDL_Event event;
    int running;

    setupVideo(this);
    this->board = NEW(Board);
    this->board->loadLevel(this->board);

    running = 1;
    while (running)
    {
	SDL_WaitEvent(&event);
	switch (event.type)
	{
	    case SDL_USEREVENT:
		if (!event.user.code)
		    handleTick(this);
		else
		    DeliverEvent(CAST(event.user.data1, Event));
		break;
	    
	    case SDL_KEYDOWN:
	    case SDL_KEYUP:
		handleKeyboardEvent(this, &event.key);
		break;

	    case SDL_QUIT:
		running = 0;
		break;

	}
    }
}

static void
m_abort ARG()
{
    METHOD(Stoneage);

    Stoneage_dtor(this);
    exit(-1);
}

CTOR(Stoneage)
{
    BASECTOR(Stoneage, App);

    Resfile rf;
    Resource r;
    SDL_RWops *rw;
    SDL_Surface *icon;
    Uint32 colkey;

    ((App)this)->run = &m_run;
    ((App)this)->abort = &m_abort;
    SDL_Init(
	    SDL_INIT_TIMER | SDL_INIT_VIDEO
	    );
    this->res_x = 1024;
    this->res_y = 768;
    this->bpp = 32;
    this->modeflags = 0;

    rf = NEW(Resfile);
    rf->setFile(rf, RES_GFX);
    if (rf->open(rf, 0) < 0)
    {
	log_err("Error loading icon.\n");
	mainApp->abort(mainApp);
    }
    rf->load(rf, "mainicon", &r);
    DELETE(Resfile, rf);

    rw = SDL_RWFromMem((void *)r->getData(r), r->getDataSize(r));
    icon = SDL_LoadBMP_RW(rw, 0);
    DELETE(Resource, r);
    SDL_FreeRW(rw);
    colkey = 0;
    memcpy(&colkey, (char *)icon->pixels, icon->format->BytesPerPixel);
    SDL_SetColorKey(icon, SDL_SRCCOLORKEY, colkey);
    SDL_WM_SetIcon(icon, 0);
    SDL_FreeSurface(icon);

    SDL_WM_SetCaption("Stonage 0.1 -- as seen 1988 in AmigaBASIC", "stoneage");

    this->ticker = SDL_AddTimer(1000, &createTickerEvent, this);
    return this;
}

DTOR(Stoneage)
{
    DELETE(Board, this->board);
    SDL_RemoveTimer(this->ticker);
    SDL_Quit();
    BASEDTOR(App);
}

int
main(int argc, char **argv)
{
    mainApp = (App)NEW(Stoneage);
    mainApp->run(mainApp, argc, argv);
    DELETE(Stoneage, mainApp);
}

