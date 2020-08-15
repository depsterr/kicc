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
    char** paths = 0;
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
            paths = xrealloc(paths, pathn * sizeof(char*));
            paths[pathn - 1] = path;
        }
        closedir(d);
    }

    /* null byte */
    paths = xrealloc(paths, (sizeof(char*)) * (pathn + 1));
    paths[pathn] = (char*)0;

    return paths;
}

char** get_package_paths(char* pkg) {
    char** paths = 0;
    int pathn = 0;
    for (int n = 0; KISS_PATH[n]; n++) {
        DIR *d;
        struct dirent *ent;
        d = opendir(KISS_PATH[n]);
        if (d) {
            while ((ent = readdir(d)) != NULL) {
                if (!strcmp(ent->d_name, pkg)) {
                    char* path = xmalloc(strlen(KISS_PATH[n]) + strlen(pkg) + 2);
                    strcpy(path, KISS_PATH[n]);
                    strapp(path, '/');
                    strcat(path, pkg);

                    struct stat statbuf;
                    if (stat(path, &statbuf) || !S_ISDIR(statbuf.st_mode)) {
                        free(path);
                        continue;
                    }

                    pathn++;
                    paths = xrealloc(paths, pathn * sizeof(char*));
                    paths[pathn - 1] = path;
                }
            }
            closedir(d);
        }
    }

    /* null byte */
    paths = xrealloc(paths, (sizeof(char*)) * (pathn + 1));
    paths[pathn] = (char*)0;

    return paths;
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
