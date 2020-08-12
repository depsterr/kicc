/* licensed under The MIT License */
/* function comments in header file */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "kicc-utils.h"

void _m(const char* t, const char *f, const char *fu, const int l, const char *fmt, ...) {

    va_list args;

    va_start(args, fmt);

    printf("[%s] (%s:%s:%d) ", t, fu, f, l);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}

int match_strings(const char* comp, unsigned int count, ...) {

    va_list va;
    va_start(va, count);

    for (unsigned int n = 0; n < count; n++)
        if (!strcmp(va_arg(va, char*), comp)) {
            va_end(va);
            return n + 1;
        }

    va_end(va);
    return -1;
}

char** get_kiss_extentions(void) {
    die("TODO");
    return 0;
}

void usage_and_extentions(void) {
    printf(
            CLR_YELLOW "->" CLR_CLEAR " kiss [a|b|c|d|i|l|r|s|u|v] [pkg]...\n"
            CLR_YELLOW "->" CLR_CLEAR " alternatives List and swap to alternatives\n"
            CLR_YELLOW "->" CLR_CLEAR " build        Build a package\n"
            CLR_YELLOW "->" CLR_CLEAR " checksum     Generate checksums\n"
            CLR_YELLOW "->" CLR_CLEAR " download     Pre-download all sources\n"
            CLR_YELLOW "->" CLR_CLEAR " install      Install a package\n"
            CLR_YELLOW "->" CLR_CLEAR " list         List installed packages\n"
            CLR_YELLOW "->" CLR_CLEAR " remove       Remove a package\n"
            CLR_YELLOW "->" CLR_CLEAR " search       Search for a package\n"
            CLR_YELLOW "->" CLR_CLEAR " update       Update the system\n"
            CLR_YELLOW "->" CLR_CLEAR " version      Package manager version\n"
            CLR_YELLOW "->" CLR_CLEAR " Installed extensions (kiss-* in PATH)\n\n"
            );

    char** extentions;
    extentions = get_kiss_extentions();
}
