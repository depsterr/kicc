#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "kicc-utils.h"
#include "kicc-build.h"

void build(char** pkg_names, int pkg_count) {

    printf(CLR_YELLOW "->" CLR_CLEAR " Resolving dependencies\n");

    pkg_names = add_depends(pkg_names, &pkg_count);

    /* using a printf here broke the code and made this run after the */
    /* for loop on gcc, I don't know how that could happen, and I */ 
    /* think gnu are insane */

    char buildstr[] = CLR_YELLOW "->" CLR_CLEAR " Building: ";
    write(1, buildstr, strlen(buildstr));

    for (int n = 0; n < pkg_count; n++) {
        write(1, pkg_names[n], strlen(pkg_names[n]));
        write(1, " ", 1);
    }
    write(1, "\n", 1);

    if (pkg_count > 1) {
        /* same issue with printf not printing in timeand having to */
        /* use write  */
        char promptstr[] = CLR_YELLOW "->" CLR_CLEAR " Continue?: Press Enter to continue or Ctrl+C to abort here\n";
        write(1, promptstr, strlen(promptstr));
        char r;
        if (!read(0, &r, 1))
            exit(0);
    }

    /* TODO: trap signals to cleanup */

    for (int n = 0; n < pkg_count; n++) {
        /* build_pkg(pkg_names[n]); */
    }

    die("TODO");
}

char** add_depends(char** pkg_names, int* pkg_count) {
    /* set up heap array since we're dealing with argv which */
    /* can't be reallocated */
    char** pkgs = xmalloc(*pkg_count * sizeof(char*));
    for (int n = 0; n < *pkg_count; n++) {
        pkgs[n] = xmalloc(strlen(pkg_names[n]));
        strcpy(pkgs[n], pkg_names[n]);
    }

    /* TODO look for depends (lots more util stuff to do before this) */

    return pkgs;
}
