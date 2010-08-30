#include <string.h>

#include "stoneage.h"
#include "screen.h"
#include "board.h"
#include "event.h"
#include "ewilly.h"

typedef enum
{
    TT_None,
    TT_KeyCombine
} TickerType;

static Uint8 *keyState;

struct Stoneage_impl
{
    int paused;
    Uint32 lastTimerTicks;
    Uint32 remainingTimerTicks;

    SDL_TimerID ticker;
    SDL_TimerID keyCheck;
};

Uint32
createTickerEvent(Uint32 interval, void *param)
{
    Stoneage this = CAST(param, Stoneage);

    SDL_Event e;
    SDL_UserEvent ue;

    this->pimpl->lastTimerTicks = SDL_GetTicks();
    ue.type = SDL_USEREVENT;
    ue.code = 0;
    ue.data1 = 0;
    ue.data2 = (void *)TT_None;
    e.type = SDL_USEREVENT;
    e.user = ue;
    SDL_PushEvent(&e);
    return 1000;
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
    Screen s = getScreen();

    this->modeflags ^= (SDL_HWSURFACE|SDL_FULLSCREEN);
    setupVideo(this);
    s->initVideo(s);
}

static void
moveWilly(Stoneage this, int x, int y)
{
    Event ev;
    MoveData md;

    md = XMALLOC(struct MoveData, 1);
    md->x = x;
    md->y = y;
    ev = NEW(Event);
    ev->type = SAEV_Move;
    ev->sender = CAST(this, Object);
    ev->handler = CAST(getWilly(), EHandler);
    ev->data = md;
    RaiseEvent(ev);
}

Uint32
combineKey(Uint32 interval, void *param)
{
    /*
    Stoneage this = CAST(param, Stoneage);
    */

    SDL_Event e;
    SDL_UserEvent ue;
    ue.type = SDL_USEREVENT;
    ue.code = 0;
    ue.data1 = 0;
    ue.data2 = (void *)TT_KeyCombine;
    e.type = SDL_USEREVENT;
    e.user = ue;
    SDL_PushEvent(&e);
    return interval;
}

static void
checkKeys(Stoneage this)
{
    int x, y;

    SDL_PumpEvents();
    x = keyState[SDLK_RIGHT] - keyState[SDLK_LEFT];
    y = keyState[SDLK_DOWN] - keyState[SDLK_UP];
    if (x||y)
	moveWilly(this, x, y);
    else
    {
	SDL_RemoveTimer(this->pimpl->keyCheck);
	this->pimpl->keyCheck = 0;
    }
}

static void
handleKeyboardEvent(Stoneage this, SDL_KeyboardEvent *e)
{
    Screen s;
    Board b;

    struct Stoneage_impl *simpl = this->pimpl;

    if (e->state == SDL_PRESSED)
    {
	if (e->keysym.mod & KMOD_ALT)
	{
	    switch (e->keysym.sym)
	    {
		case SDLK_RETURN:
		    toggleFullscreen(this);
		    break;
		default:
		    ;
	    }
	}
	else if (e->keysym.mod & KMOD_CTRL)
	{
	    switch (e->keysym.sym)
	    {
		case SDLK_n:
		    s = getScreen();
		    b = s->getBoard(s);
		    b->loadLevel(b, -1);
		    break;
		default:
		    ;
	    }
	}
	else
	{
	    switch (e->keysym.sym)
	    {
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_LEFT:
		case SDLK_RIGHT:
		    if (!simpl->keyCheck)
			simpl->keyCheck = SDL_AddTimer(30, &combineKey, this);
		    break;
		case SDLK_p:
		    simpl->paused ^= 1;
		    if (simpl->paused)
		    {
			SDL_RemoveTimer(simpl->ticker);
			simpl->remainingTimerTicks = 1000 -
			    ( SDL_GetTicks() - simpl->lastTimerTicks );
		    }
		    else
		    {
			simpl->lastTimerTicks =
			    SDL_GetTicks() - simpl->remainingTimerTicks;
			simpl->ticker = SDL_AddTimer(
				simpl->remainingTimerTicks,
				&createTickerEvent, this);
		    }
		    break;
		default:
		    ;
	    }
	}
    }
}

static void
handleTick(Stoneage this)
{
    Screen s = getScreen();

    s->timeStep(s);
}

static int
m_run(THIS, int argc, char **argv)
{
    METHOD(Stoneage);

    SDL_Event event;
    int running;
    Screen s;

    s = getScreen();
    setupVideo(this);
    s->initVideo(s);
    s->startGame(s);

    running = 1;
    keyState = SDL_GetKeyState(0);
    while (running)
    {
	SDL_WaitEvent(&event);
	switch (event.type)
	{
	    case SDL_USEREVENT:
		if (!event.user.code)
		    switch ((TickerType)event.user.data2)
		    {
			case TT_None:
			    handleTick(this);
			    break;
			case TT_KeyCombine:
			    checkKeys(this);
			    break;
		    }
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
    return 0;
}

static void
m_abort(THIS)
{
    METHOD(Stoneage);

    Stoneage_dtor(this);
    exit(-1);
}

CTOR(Stoneage)
{
    struct Stoneage_impl *s;

    BASECTOR(Stoneage, App);

    s = XMALLOC(struct Stoneage_impl, 1);
    memset(s, 0, sizeof(struct Stoneage_impl));
    this->pimpl = s;

    ((App)this)->run = &m_run;
    ((App)this)->abort = &m_abort;
    SDL_Init(
	    SDL_INIT_TIMER | SDL_INIT_VIDEO
	    );
    this->res_x = 1024;
    this->res_y = 768;
    this->bpp = 32;
    this->modeflags = 0;

    setupSdlApplicationIcon();
    SDL_WM_SetCaption("Stonage " VERSION
	    " -- as seen 1988 in AmigaBASIC", "stoneage");

    s->ticker = SDL_AddTimer(1000, &createTickerEvent, this);
    s->keyCheck = 0;
    return this;
}

DTOR(Stoneage)
{
    SDL_RemoveTimer(this->pimpl->ticker);
    XFREE(this->pimpl);
    SDL_Quit();
    BASEDTOR(App);
}

int
main(int argc, char **argv)
{
    int rc;

    mainApp = (App)NEW(Stoneage);
    rc = mainApp->run(mainApp, argc, argv);
    DELETE(Stoneage, mainApp);
    return rc;
}

