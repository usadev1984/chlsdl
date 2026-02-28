#include "string.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

char *
svconcat(const char * fmt, ...)
{
    va_list p;
    va_start(p, fmt);

    char * s = malloc(vsnprintf(NULL, 0, fmt, p) + 1);
    if (!s)
        return NULL;

    va_end(p);

    va_start(p, fmt);

    vsprintf(s, fmt, p);
    /* assert(s); */

    va_end(p);

    return s;
}
