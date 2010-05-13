#include "common.h"
#include "entity.h"
#include "stoneage.h"

Stoneage mainApp;

// methods:
void Stoneage_run(void *s);
void Stoneage_abort(void *s);


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

void Stoneage_run(void *s)
{
    METHOD(Stoneage, s);

    this->screen = SDL_SetVideoMode(640, 480, 16, 0);
    if (!this->screen)
    {
	log_err("Unable to set video mode: %s\n", SDL_GetError());
	this->abort(this);
    }
}

void Stoneage_abort(void *s)
{
    METHOD(Stoneage, s);

    Stoneage_dtor(this);
    exit(-1);
}

