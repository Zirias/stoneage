#include "common.h"
#include "app.h"

void *
xmalloc(size_t n)
{
    void *p;
   
    p = malloc(n);
    if (!p)
    {
	log_err(ERRMSG_OOM);
	mainApp->abort(mainApp);
    }
    return p;
}

void *
xrealloc(void *p, size_t n)
{
    void *rp;

    if (!p) return xmalloc(n);
    rp = realloc(p, n);
    if (!rp)
    {
	free(p);
	log_err(ERRMSG_OOM);
	mainApp->abort(mainApp);
    }
    return rp;
}

