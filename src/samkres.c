#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "samkres.h"

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

    Resource r;
    Resfile rf;
    FILE *file;
    struct stat s;
    size_t size;

    if (argc != 4)
    {
	fprintf(stderr, "USAGE: %s [resfile] [file] [key]\n", argv[0]);
	return -1;
    }

    rf = this->resfile;

    rf->setFile(rf, argv[1]);
    if (rf->open(rf, 1) < 0)
    {
	fprintf(stderr, "Unable to write %s.\n", argv[1]);
	return -1;
    }

    file = fopen(argv[2], "rb");
    if (!file)
    {
	fprintf(stderr, "Could not read %s.\n", argv[2]);
	return -1;
    }

    fstat(fileno(file), &s);
    size = (size_t) s.st_size;
    r = NEW(Resource);
    r->setName(r, argv[3]);
    r->readDataFrom(r, file, size);
    fclose(file);

    if (rf->store(rf, r) < 0)
    {
	fprintf(stderr, "Could not store resource %s.\n"
		"maybe it's already there?\n", argv[3]);
	return -1;
    }
    return 0;
}

CTOR(SaMkres)
{
    BASECTOR(SaMkres, App);
    ((App)this)->run = &m_run;
    ((App)this)->abort = &m_abort;
    this->resfile = NEW(Resfile);
    return this;
}

DTOR(SaMkres)
{
    DELETE(Resfile, this->resfile);
    BASEDTOR(App);
}

int
main(int argc, char **argv)
{
    mainApp = (App)NEW(SaMkres);
    int rc = mainApp->run(mainApp, argc, argv);
    DELETE(SaMkres, mainApp);
    return rc;
}

