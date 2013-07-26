#include <stdio.h>
#include <string.h>
#include "help.h"
#include "global.h"

extern struct cmd_struct rss_commands[];

void usage(void) {
        printf("usage: rss help command\n");
}

int cmd_help(int argc, char **argv) {
        struct cmd_struct *ptr;
        if (argc != 2) {
                fprintf(stderr, "usage: rss help command.\n");
                return -1;
        }
        if (strcmp(argv[1],"help") == 0) {
                usage();
                return 0;
        }
        ptr = rss_commands;
        while (ptr->cmd) {
                if (strcmp(argv[1],ptr->cmd) == 0) {
                        strcpy(argv[0], argv[1]);
                        strcpy(argv[1],"-h");
                        ptr->fn(argc, argv);
                        break;
                }
                ptr++;
        }
        if (!ptr->cmd) {
                fprintf(stderr, "fatal: %s: no such command.\n", argv[1]);
                return -1;
        }
        return 0;
}

