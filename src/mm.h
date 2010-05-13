#ifndef STONEAGE_MM_H
#define STONEAGE_MM_H

#include <stdlib.h>

#define XMALLOC(T, n) ((T *)xmalloc((n) * sizeof(T)))
#define XREALLOC(T, p, n) ((T *)xrealloc((p), (n) * sizeof(T)))
#define XFREE(p) do { if (p) {free(p); p=0;}} while(0)

extern void *xmalloc(size_t n);
extern void *xrealloc(void *p, size_t n);

#endif
