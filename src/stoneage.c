#include "common.h"
#include "entity.h"
#include "stoneage.h"

Stoneage mainApp;

void
m_run ARG()
{
    METHOD(Stoneage);

    this->screen = SDL_SetVideoMode(640, 480, 16, 0);
    if (!this->screen)
    {
	log_err("Unable to set video mode: %s\n", SDL_GetError());
	this->abort(this);
    }
}

void
m_abort ARG()
{
    METHOD(Stoneage);

    Stoneage_dtor(this);
    exit(-1);
}

CTOR(Stoneage)
{
    BASECTOR(Stoneage, Object);
    this->run = &m_run;
    this->abort = &m_abort;
    return this;
}

DTOR(Stoneage)
{
    SDL_Quit();
    BASEDTOR(Object);
}

int
main(int argc, char **argv)
{
    mainApp = NEW(Stoneage);
    mainApp->run(mainApp);
    DELETE(Stoneage, mainApp);
}

