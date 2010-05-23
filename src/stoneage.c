#include "stoneage.h"
#include "board.h"
#include "event.h"
#include "ewilly.h"

static int m_x, m_y;

typedef enum
{
    TT_None,
    TT_KeyCombine
} TickerType;

Uint32
createTickerEvent(Uint32 interval, void *param)
{
    Stoneage this = CAST(param, Stoneage);

    SDL_Event e;
    SDL_UserEvent ue;
    ue.type = SDL_USEREVENT;
    ue.code = 0;
    ue.data1 = 0;
    ue.data2 = (void *)TT_None;
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
    Stoneage this = CAST(param, Stoneage);

    SDL_Event e;
    SDL_UserEvent ue;
    ue.type = SDL_USEREVENT;
    ue.code = 0;
    ue.data1 = 0;
    ue.data2 = (void *)TT_KeyCombine;
    e.type = SDL_USEREVENT;
    e.user = ue;
    SDL_PushEvent(&e);
    return 0;
}

static void
checkKeys(Stoneage this)
{
    if (m_x || m_y)
    {
	moveWilly(this, m_x, m_y);
	m_x = m_y = 0;
    }
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
	else
	{
	    switch (e->keysym.sym)
	    {
		case SDLK_UP:
		    if (m_x < 0)
		    {
			m_x = 0;
			moveWilly(this, -1,-1);
		    }
		    else if (m_x > 0)
		    {
			m_x = 0;
			moveWilly(this, 1,-1);
		    }
		    else
		    {
			m_y = -1;
			SDL_AddTimer(50, &combineKey, this);
		    }
		    break;
		case SDLK_DOWN:
		    if (m_x < 0)
		    {
			m_x = 0;
			moveWilly(this, -1,1);
		    }
		    else if (m_x > 0)
		    {
			m_x = 0;
			moveWilly(this, 1,1);
		    }
		    else
		    {
			m_y = 1;
			SDL_AddTimer(50, &combineKey, this);
		    }
		    break;
		case SDLK_LEFT:
		    if (m_y < 0)
		    {
			m_y = 0;
			moveWilly(this, -1,-1);
		    }
		    else if (m_y > 0)
		    {
			m_y = 0;
			moveWilly(this, -1,1);
		    }
		    else
		    {
			m_x = -1;
			SDL_AddTimer(50, &combineKey, this);
		    }
		    break;
		case SDLK_RIGHT:
		    if (m_y < 0)
		    {
			m_y = 0;
			moveWilly(this, 1,-1);
		    }
		    else if (m_y > 0)
		    {
			m_y = 0;
			moveWilly(this, 1,1);
		    }
		    else
		    {
			m_x = 1;
			SDL_AddTimer(50, &combineKey, this);
		    }
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
m_run(THIS, int argc, char **argv)
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

    setupSdlApplicationIcon();
    SDL_WM_SetCaption("Stonage " VERSION
	    " -- as seen 1988 in AmigaBASIC", "stoneage");

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

