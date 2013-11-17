#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "global.h"
#include "add.h"
#include "fetch.h"
#include "help.h"
#include "init.h"
#include "list.h"
#include "remove.h"
#include "show.h"
#include "parse.h"

struct cmd_struct rss_commands[] = {
        {"add", cmd_add, NEED_RSS_DIR},
        {"fetch", cmd_fetch, NEED_RSS_DIR},
        {"help", cmd_help, NONE},
        {"init", cmd_init, NONE},
        {"list", cmd_list, NEED_RSS_DIR},
        {"remove", cmd_remove, NEED_RSS_DIR},
        {"show", cmd_show, NEED_RSS_DIR},
        {NULL, NULL, NONE}
};

static int handle_internal_command(int argc, char **argv) {
        int i;
        char *cmd = argv[0];
        struct stat buf;
        i = 0;
        while (rss_commands[i].cmd) {
                if (strcmp(cmd,rss_commands[i].cmd) == 0) {
                        if (rss_commands[i].option & NEED_RSS_DIR && stat(RSS_DIR, &buf) == -1) {
                                if (errno == ENOENT)
                                        fprintf(stderr, "fatal: %s: no such directory.\n"
                                                        "did you initialise this directory with 'rss init'?\n", RSS_DIR);
                                else
                                        perror("fatal:");
                                return -1;

                        }
                        return rss_commands[i].fn(argc, argv);
                }
                i++;
        }
        fprintf(stderr, "fatal: %s: unknown command.\n", cmd);
        return -1;
}

int main(int argc, char **argv) {
        int err;
        char *cmd;
        cmd = argv[0];

        //parse_xml("sample-rss-2.xml");

        if (!prefixcmp(cmd,"rss-")) {
                cmd += 4;
                argv[0] = cmd;
                err = handle_internal_command(argc, argv);
        }
        else if (argc == 1) {
                err = cmd_help(0, NULL);
        }
        else {
                err = handle_internal_command(argc-1, argv+1);
        }

        return err;
}

