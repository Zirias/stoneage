#include "app.h"

App mainApp;

CTOR(App)
{
    BASECTOR(App, Object);
    this->run = 0;
    this->abort = 0;
    return this;
}

DTOR(App)
{
    BASEDTOR(Object);
}

