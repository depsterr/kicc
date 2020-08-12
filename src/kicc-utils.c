/* licensed under The MIT License */
/* function comments in header file */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "kiss-utils.h"

void _m(const char* t, const char *f, const int l, const char *fmt, ...) {

    va_list args;

    va_start(args, fmt);

    printf("[%s] (%s:%d) ", t, f, l);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}

int match_strings(const char* comp, unsigned int count, ...) {

    va_list va;
    va_start(va, count);

    int len = strlen(comp);
    for (int n = 0; n < count; n++)
        if (!strcmp(va_arg(va, char*), comp)) {
            va_end(va);
            return n;
        }


    va_end(va);
    return -1;
}
