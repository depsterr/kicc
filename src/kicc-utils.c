/* licensed under The MIT License */
/* function comments in header file */
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>

#include "kicc-utils.h"
#include "kicc-globals.h"

void _m(const char* t, const char *f, const char *fu, const int l, const char *fmt, ...) {

    va_list args;

    va_start(args, fmt);

    printf("[%s] (%s:%s:%d) ", t, fu, f, l);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}

char* xgetenv(char* name, char* alt) {
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

void strapp(char* str, char c) {
    int len = strlen(str);
    str[len] = c;
    str[len+1] = '\0';
}

void ready_kiss_path(void) {
    if (KISS_PATH)
        return;

    char* kp;
    kp = getenv("KISS_PATH");
    if (!kp)
        /* TODO don't just fail here, some commands do not need KISS_PATH, perhaps */
        /* set it up when needed instead. */
        die("KISS_PATH not set");

    /* amount of paths */
    int npaths = 1;
    for (int n = 0; kp[n]; n++)
        if (kp[n] == ':')
            npaths++;

    /* memory for array of paths (and null terminator) */
    KISS_PATH = xmalloc((npaths + 1) * sizeof(char*));
    KISS_PATH[npaths] = (char*)0;

    /* copy string since string from get_env is not editable */
    char* p = xmalloc(strlen(kp) + 1);
    strcpy(p, kp);
    kp = p;

    /* p is the start of the next path */
    npaths = 0;
    for (int n = 0;; n++) {
        if (kp[n] == ':') {
            KISS_PATH[npaths] = p;
            kp[n] = '\0';
            p = &kp[n+1];
            npaths++;
        } else if (!kp[n]) {
            KISS_PATH[npaths] = p;
            break;
        }
    }
}

char** get_kiss_extentions(void) {
    char* path = xgetenv("PATH", "/bin/");
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
                    strapp(extentions[extn - 1], '/');
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

char** get_installed_packages(void) {
    char** package_paths = 0;
    int pathn = 0;
    DIR *d;
    struct dirent *ent;
    d = opendir(sys_db);
    if (d) {
        while ((ent = readdir(d)) != NULL) {
            /* skip hidden entries */
            if (ent->d_name[0] == '.')
                continue;
            /* TODO make global vars string size global to avoid */
            /* multiple strlens on the same string */
            char* path = xmalloc(strlen(ent->d_name) + strlen(sys_db) + 2);
            strcpy(path, sys_db);
            strapp(path, '/');
            strcat(path, ent->d_name);

            struct stat statbuf;
            if (stat(path, &statbuf) || !S_ISDIR(statbuf.st_mode)) {
                free(path);
                continue;
            }

            pathn++;
            package_paths = xrealloc(package_paths, pathn * sizeof(char*));
            package_paths[pathn - 1] = path;
        }
        closedir(d);
    } else
        die("could not open package database: '%s'", sys_db);

    /* null byte */
    package_paths = xrealloc(package_paths, (sizeof(char*)) * (pathn + 1));
    package_paths[pathn] = (char*)0;

    return package_paths;
}

bool is_installed(char* pkg) {
    char** pkgs = get_installed_packages();
    bool ret = false;
    for (int n = 0; pkgs[n]; n++) {
        if(!strcmp(strrchr(pkgs[n], '/') + 1, pkg))
            ret = true;
        free(pkgs[n]);
    }
    free(pkgs);
    return ret;
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
