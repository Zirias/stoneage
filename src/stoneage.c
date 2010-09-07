#include <string.h>

#include "stoneage.h"
#include "screen.h"
#include "board.h"
#include "event.h"
#include "ewilly.h"

static Uint8 *keyState;

struct Stoneage_impl
{
    int paused;
    Uint32 lastTimerTicks;
    Uint32 remainingTimerTicks;

    SDL_TimerID ticker;
    SDL_TimerID keyCheckTicker;

    Event KeyCheck;
};

Uint32
createTickerEvent(Uint32 interval, void *param)
{
    Stoneage this = CAST(param, Stoneage);

    this->pimpl->lastTimerTicks = SDL_GetTicks();
    RaiseEvent(this->Tick, (Object)this, 0);
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
moveWilly(Stoneage this, int dx, int dy)
{
    MoveWillyEventData *md;

    md = XMALLOC(MoveWillyEventData, 1);
    md->dx = dx;
    md->dy = dy;
    RaiseEvent(this->MoveWilly, (Object)this, md);
}

Uint32
combineKey(Uint32 interval, void *param)
{
    Stoneage this = CAST(param, Stoneage);

    RaiseEvent(this->pimpl->KeyCheck, (Object)this, 0);
    return interval;
}

static void
Stoneage_KeyCheck(THIS, Object sender, void *data)
{
    METHOD(Stoneage);

    int x, y;

    x = keyState[SDLK_RIGHT] - keyState[SDLK_LEFT];
    y = keyState[SDLK_DOWN] - keyState[SDLK_UP];
    if (x||y)
	moveWilly(this, x, y);
    else
    {
	SDL_RemoveTimer(this->pimpl->keyCheckTicker);
	this->pimpl->keyCheckTicker = 0;
    }
}

static void
handlePause(Stoneage this)
{
    Screen s;
    Board b;

    struct Stoneage_impl *simpl = this->pimpl;

    s = getScreen();
    b = s->getBoard(s);
    if (simpl->paused)
    {
	SDL_RemoveTimer(simpl->ticker);
	simpl->remainingTimerTicks = 1000 -
	    ( SDL_GetTicks() - simpl->lastTimerTicks );
	b->setPaused(b, 1);
    }
    else
    {
	simpl->lastTimerTicks =
	    SDL_GetTicks() - simpl->remainingTimerTicks;
	simpl->ticker = SDL_AddTimer(
		simpl->remainingTimerTicks,
		&createTickerEvent, this);
	b->setPaused(b, 0);
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
		    if (!simpl->keyCheckTicker)
			simpl->keyCheckTicker =
			    SDL_AddTimer(30, &combineKey, this);
		    break;
		case SDLK_p:
		    simpl->paused ^= 1;
		    handlePause(this);
		    break;
		default:
		    ;
	    }
	}
    }
}

static void
Event_SDLEvent(THIS, Object sender, void *data)
{
    METHOD(Stoneage);

    SDL_Event *ev = data;

    switch (ev->type)
    {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
	    handleKeyboardEvent(this, &(ev->key));
	    break;

	case SDL_ACTIVEEVENT:
	    if ((ev->active.gain == 0)
		    && (ev->active.state == SDL_APPINPUTFOCUS)
		    && (! this->pimpl->paused))
	    {
		this->pimpl->paused = 1;
		handlePause(this);
	    }
	    break;
	case SDL_QUIT:
	    DoneEvents();
	    break;
    }
}

static int
m_run(THIS, int argc, char **argv)
{
    METHOD(Stoneage);

    Screen s;

    s = getScreen();
    setupVideo(this);
    s->initVideo(s);
    s->startGame(s);

    keyState = SDL_GetKeyState(0);
    DoEventLoop();
    DELETE(Screen, s);
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

    InitEvents();
    s->KeyCheck = CreateEvent();
    this->MoveWilly = CreateEvent();
    this->Tick = CreateEvent();

    AddHandler(s->KeyCheck, this, &Stoneage_KeyCheck);
    AddHandler(SDLEvent, this, &Event_SDLEvent);

    setupSdlApplicationIcon();
    SDL_WM_SetCaption("Stonage " VERSION
	    " -- as seen 1988 in AmigaBASIC", "stoneage");

    s->ticker = SDL_AddTimer(1000, &createTickerEvent, this);
    s->keyCheckTicker = 0;
    return this;
}

DTOR(Stoneage)
{
    DestroyEvent(this->pimpl->KeyCheck);
    DestroyEvent(this->Tick);
    DestroyEvent(this->MoveWilly);
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

