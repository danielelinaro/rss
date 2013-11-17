#include <stdio.h>
#include <string.h>
#include "help.h"
#include "global.h"

extern struct cmd_struct rss_commands[];

const char rss_help_usage_string[] = 
        "Usage: rss <command> [<args>]\n\n"
        "The following commands are available:\n"
        "    add       add a feed to the database\n"
        "    fetch     fetch one or more feeds\n"
        "    init      initialise the database\n"
        "    list      list the feeds currently stored in the database\n"
        "    remove    remove a feed from the database\n"
        "    show      show a feed present in the database\n"
        "\nSee 'rss help <command>' for more information on a specific command.\n";

void usage(void) {
        printf("%s", rss_help_usage_string);
}

int cmd_help(int argc, char **argv) {
        struct cmd_struct *ptr;
        if (argc != 2) {
                usage();
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

