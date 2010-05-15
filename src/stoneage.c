#include "entity.h"
#include "stoneage.h"

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
    }
}

static int
m_run ARG(int argc, char **argv)
{
    METHOD(Stoneage);
    SDL_Event event;
    int running;

    SDL_WM_SetCaption("Stonage 0.1 -- as seen 1988 in AmigaBASIC", "stoneage");

    setupVideo(this);
    this->board = NEW(Board);

    running = 1;
    while (running)
    {
	SDL_WaitEvent(&event);
	switch (event.type)
	{
	    case SDL_USEREVENT:
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
    ((App)this)->run = &m_run;
    ((App)this)->abort = &m_abort;
    SDL_Init(
	    SDL_INIT_TIMER | SDL_INIT_VIDEO
	    );
    this->res_x = 1024;
    this->res_y = 768;
    this->bpp = 32;
    this->modeflags = 0;
    return this;
}

DTOR(Stoneage)
{
    DELETE(Board, this->board);
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

