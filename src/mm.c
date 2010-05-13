#include "common.h"

void *
xmalloc(size_t n)
{
    void *p = malloc(n);
    if (!p)
    {
	log_err(ERRMSG_OOM);
	exit(-1);
    }
    return p;
}

void *
xrealloc(void *p, size_t n)
{
    if (!p) return xmalloc(n);
    void *rp = realloc(p, n);
    if (!rp)
    {
	free(p);
	log_err(ERRMSG_OOM);
	exit(-1);
    }
    return rp;
}

