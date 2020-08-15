#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "kicc-utils.h"
#include "kicc-build.h"

/* globals */
char** KISS_PATH;
char* KISS_ROOT;
char* pkg_db;
char* sys_db;

/* TODO, set up some sort of exit function which clears globals (freeing memory */
/* before exiting is good practice, even though it's redundant on most sane */
/* systems.) */

void set_globals(void);

void set_globals(void) {
    /* set this only when needed */
    KISS_PATH = (char**)0;

    /* cannot modify memory gotten from getenv */
    KISS_ROOT = xgetenv("KISS_ROOT", "");
    char* root = xmalloc(strlen(KISS_ROOT) + 1);
    strcpy(root, KISS_ROOT);
    KISS_ROOT = root;

    /* remove trailing slash from KISS_ROOT */
    for(int len = strlen(KISS_ROOT); KISS_ROOT[len-1] == '/'; len--)
        KISS_ROOT[len-1] = '\0';

    pkg_db = xgetenv("pkg_db", "var/db/kiss/installed");

    /* +2 leaves space for slash and nullbyte */
    sys_db = xmalloc(strlen(pkg_db) + strlen(KISS_ROOT) + 2);
    strcpy(sys_db, KISS_ROOT);
    strapp(sys_db, '/');
    strcat(sys_db, pkg_db);
}

int main(int argc, char** argv) {
    if(!argv[1])
        usage_and_extentions();

    set_globals();

    switch(match_strings(argv[1], 20,
                "alternatives", "a",
                "build", "b",
                "checksum", "c",
                "download", "d",
                "install", "i",
                "list", "l",
                "remove", "r",
                "search", "s",
                "update", "u",
                "version", "v")) {
        /* alternatives */
        case 1:
        case 2:
            die("alternatives");
            break;
        /* build */
        case 3:
        case 4:
            if (argv[2])
                build(&argv[2], argc - 2);
            else
                build(&"kicc", 1);
            exit(0);
            break;
        /* checksum */
        case 5:
        case 6:
            die("checksum");
            break;
        /* download */
        case 7:
        case 8:
            die("download");
            break;
        /* install */
        case 9:
        case 10:
            die("install");
            break;
        /* list */
        case 11:
        case 12:;
            char** packages = get_installed_packages();
            int fp;
            char version[32];
            char* path;
            size_t size;
            for (int n = 0; packages[n]; n++) {
                /* space for /, \0 and version */
                path = xmalloc(strlen(packages[n]) + 9);
                strcpy(path, packages[n]);
                strapp(path, '/');
                strcat(path, "version");

                if (!access(path, F_OK|R_OK)) {
                    fp = open(path, O_RDONLY);
                    size = read(fp, version, 32);
                    version[size - 1] = '\0';
                } else
                    version[0] = '\0';

                printf("%s %s\n", strrchr(packages[n], '/') + 1, version);

                /* free memory before exiting just to be sure */
                free(path);
                free(packages[n]);
            }

            /* free memory before exiting just to be sure */
            free(packages);
            exit(0);
        /* remove */
        case 13:
        case 14:
            die("remove");
            break;
        /* search */
        case 15:
        case 16:
            die("search");
            break;
        /* update */
        case 17:
        case 18:
            die("update");
            break;
        /* version */
        case 19:
        case 20:
            printf(VERSION "\n");
            exit(0);
        /* kiss extention */
        default: ;
            char** extentions;
            extentions = get_kiss_extentions();
            int e = -1;


            for (int n = 0; extentions[n]; n++) {
                /* compare basename, + 6 for skipping over '/kiss-'*/
                if (!strcmp(strrchr(extentions[n], '/') + 6, argv[1])) {
                    e = n;
                    break;
                }
            }

            if (e < 0) {
                printf(CLR_RED "ERROR" CLR_CLEAR " '%s' is not a valid command\n", argv[1]);
                exit(1);
            }

            char* extention_argv[argc];
            extention_argv[0] = extentions[e];

            for (int n = 2; n < argc; n++) {
                extention_argv[n - 1] = argv[n];
            }

            extention_argv[argc - 1] = (char*)0;

            pid_t cpid = fork();
            if (cpid == 0) {
                setsid();
                execvp(extention_argv[0], extention_argv);
                die("execve failed for %s", extention_argv[0]);
            }

            for (int n = 0; extentions[n]; n++)
                free(extentions[n]);


            int status;
            waitpid(cpid, &status, 0);

            if (!WIFEXITED(status))
                exit(1);

            exit(WEXITSTATUS(status));
    }
    die("UNRECHABLE, CONTACT DEVS!");
}
