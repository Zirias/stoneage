#include "screen.h"
#include "resfile.h"
#include "app.h"

static Screen _instance;

CTOR(Screen)
{
    BASECTOR(Screen, Object);

    this->graphics = NEW(Resfile);
    this->graphics->setFile(this->graphics, RES_GFX);
    if (this->graphics->open(this->graphics, 0) < 0)
    {
	DELETE(Resfile, this->graphics);
	log_err("Error loading graphics from `" RES_GFX "'.\n");
	mainApp->abort(mainApp);
    }

    return this;
}

DTOR(Screen)
{
    this->graphics->close(this->graphics);
    DELETE(Resfile, this->graphics);
    BASEDTOR(Object);
}

Screen
getScreen(void)
{
    if (!_instance) _instance = NEW(Screen);
    return _instance;
}
