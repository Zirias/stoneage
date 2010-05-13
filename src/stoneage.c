#include "common.h"
#include "entity.h"
#include "stoneage.h"

Stoneage mainApp;

int
main(int argc, char **argv)
{
    mainApp = NEW(Stoneage);
    mainApp->run(mainApp);
    DELETE(Stoneage, mainApp);
}

Stoneage
Stoneage_ctor(Stoneage s, TypeList types)
{
    OBJECT_INIT(s, Stoneage);
    s->run = &Stoneage_run;
    s->abort = &Stoneage_abort;
}

void
Stoneage_dtor(Stoneage s)
{
    SDL_Quit();
    Object_dtor((Object)s);
}

void Stoneage_run(Stoneage s)
{
    s->screen = SDL_SetVideoMode(640, 480, 16, 0);
    if (!s->screen)
    {
	log_err("Unable to set video mode: %s\n", SDL_GetError());
	s->abort(s);
    }
}

void Stoneage_abort(Stoneage s)
{
    Stoneage_dtor(s);
    exit(-1);
}

