#include "samkres.h"

#include "resfile.h"

void
m_abort ARG()
{
    METHOD(SaMkres);

    SaMkres_dtor(this);
    exit(-1);
}

int
m_run ARG(int argc, char **argv)
{
    METHOD(SaMkres);

}

CTOR(SaMkres)
{
    BASECTOR(SaMkres, App);
    ((App)this)->run = &m_run;
    ((App)this)->abort = &m_abort;
    return this;
}

DTOR(SaMkres)
{
    BASEDTOR(App);
}

int
main(int argc, char **argv)
{
    mainApp = (App)NEW(SaMkres);
    DELETE(SaMkres, mainApp);
}

