#include "entity.h"
#include "stoneage.h"

int
m_run ARG(int argc, char **argv)
{
    METHOD(Stoneage);

    this->screen = SDL_SetVideoMode(640, 480, 16, 0);
    if (!this->screen)
    {
	log_err("Unable to set video mode: %s\n", SDL_GetError());
	((App)this)->abort(this);
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
    BASECTOR(Stoneage, App);
    ((App)this)->run = &m_run;
    ((App)this)->abort = &m_abort;
    return this;
}

DTOR(Stoneage)
{
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

