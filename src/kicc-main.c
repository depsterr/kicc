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

void usage_and_extentions(void);

void set_globals(void) {
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

    char* kp;
    kp = getenv("KISS_PATH");

    if (kp) {
        /* amount of paths */
        int npaths = 2;
        for (int n = 0; kp[n]; n++)
            if (kp[n] == ':')
                npaths++;

        /* memory for array of paths (plus sys_db and null terminator) */
        KISS_PATH = xmalloc((npaths + 2) * sizeof(char*));
        KISS_PATH[npaths-1] = sys_db;
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
    } else {
        /* empty path */
        KISS_PATH = xmalloc(2 * sizeof(char*));
        KISS_PATH[0] = sys_db;
        KISS_PATH[1] = (char*)0;
    }

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
