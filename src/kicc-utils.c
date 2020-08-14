/* licensed under The MIT License */
/* function comments in header file */
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "kicc-utils.h"

void _m(const char* t, const char *f, const char *fu, const int l, const char *fmt, ...) {

    va_list args;

    va_start(args, fmt);

    printf("[%s] (%s:%s:%d) ", t, fu, f, l);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}

char* xget_env(char* name, char* alt) {
    char* env = getenv(name);
    return env ? env : alt;
}

void* xmalloc(size_t size) {
    void* p = malloc(size);

    if (!p)
        die("failed to allocate memory.");

    return p;
}

void* xrealloc(void* ptr, size_t size) {
    void* p = realloc(ptr, size);

    if (!p && size)
        die("failed to reallocate memory.");

    return p;
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
    char* path = xget_env("PATH", "/bin/");
    char* colon = path;

    char** extentions = 0;
    int extn = 0;

    while (colon && *colon) {
        colon = strchr(colon, ':');
        if (colon) {
            *colon = '\0';
            colon++;
        }

        DIR *d;
        struct dirent *ent;
        d = opendir(path);
        if (d) {
            while ((ent = readdir(d)) != NULL) {
                if (!strncmp(ent->d_name, "kiss-", 5)) {
                    extn++;
                    extentions = xrealloc(extentions, (sizeof(char*)) * extn);
                    extentions[extn - 1] = xmalloc(strlen(ent->d_name) + strlen(path) + 2);
                    strcpy(extentions[extn - 1], path);
                    strcat(extentions[extn - 1], "/");
                    strcat(extentions[extn - 1], ent->d_name);
                    if(access(extentions[extn - 1], X_OK) == -1) {
                        extn--;
                        free(extentions[extn]);
                        extentions = xrealloc(extentions, (sizeof(char*)) * extn);
                    }
                }
            }
            closedir(d);
        }

        path = colon;
    }

    extentions = xrealloc(extentions, (sizeof(char*)) * (extn + 1));
    extentions[extn] = (char*)0;

    return extentions;
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
            "\n"
            CLR_YELLOW "->" CLR_CLEAR " Installed extensions (kiss-* in PATH)\n"
            );

    char** extentions;
    extentions = get_kiss_extentions();
    
    int fd;
    char desc_buffer[512];
    int size;

    int maxlen = 12;
    for (int n = 0; extentions[n]; n++) {
        size = strlen(strrchr(extentions[n], '/') + 1);
        if (size > maxlen)
            maxlen = size;
    }

    for (int n = 0; extentions[n]; n++) {
        fd = open(extentions[n], O_RDONLY);
        size = read(fd, desc_buffer, 512);
        desc_buffer[(size > 0) ? size - 1 : 0] = '\0';

        int i = 0;
        while (desc_buffer[i] != '\n' && desc_buffer[i])
            i++;
        if (desc_buffer[i]) {
            if (desc_buffer[++i] == '#') {
                i++;
                while ((desc_buffer[i] == ' ' || desc_buffer[i] == '\t') && desc_buffer[i])
                    i++;
            } else
                i = 511;
        }

        printf(CLR_YELLOW "->" CLR_CLEAR " %-*s %s\n", maxlen, strrchr(extentions[n], '/') + 1, &desc_buffer[i]);

        close(fd);
    }

    /* free memory just to be sure */
    for (int n = 0; extentions[n]; n++) {
        free(extentions[n]);
    }
    free(extentions);

    exit(0);
}
