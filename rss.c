#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "init.h"

#define prefixcmp(a,b) strncmp(a,b,strlen(b))
#define NONE 0

struct cmd_struct {
	char *cmd;
	int (*fn)(int, char **);
	int option;
};

static int handle_internal_command(int argc, char **argv) {
        int i;
        char *cmd = argv[0];
        static struct cmd_struct commands[] = {
                {"init", cmd_init, NONE},
                {NULL, NULL, NONE}
        };
        i = 0;
        while (commands[i].cmd) {
                if (strncmp(cmd,commands[i].cmd,strlen(commands[i].cmd)) == 0)
                        return commands[i].fn(argc-1, argv+1);
                i++;
        }
        fprintf(stderr, "fatal: %s: unknown command.\n", cmd);
        return -1;
}

int main(int argc, char **argv) {

        int err;
        char *cmd;
        cmd = argv[0];

        if (!prefixcmp(cmd,"rss-")) {
                cmd += 4;
                argv[0] = cmd;
                err = handle_internal_command(argc, argv);
        }
        else {
                err = handle_internal_command(argc-1, argv+1);
        }

        return err;
}

