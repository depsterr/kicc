#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "kicc-utils.h"

int main(int argc, char** argv) {
    if(!argv[1])
        usage_and_extentions();

    /* TODO set up global vars */

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
            die("build");
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
        case 12:
            die("list");
            break;
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
            break;
        /* kiss extention */
        default: ;
            char** extentions;
            extentions = get_kiss_extentions();
            int e = -1;

            for (int n = 0; extentions[n]; n++)
                /* compare basename, + 6 for skipping over '/kiss-'*/
                if (!strcmp(strrchr('/', extentions[n]) + 6, argv[1])) {
                    e = n;
                    break;
                }

            if (e < 0) {
                printf(CLR_RED "ERROR" CLR_CLEAR " '%s' is not a valid command", argv[1]);
                exit(1);
            }

            char extention_binary[strlen(extentions[e]) + 6];

            char* extention_argv[argc - 1];
            extention_argv[0] = extention_binary;

            for (int n = 2; n < argc; n++) {
                extention_argv[n - 1] = argv[n];
            }

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
}
