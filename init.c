#include <stdio.h>
#include "init.h"

int cmd_init(int argc, char **argv) {
        int i;
        for (i=0; i<argc; i++)
                printf("%s\n", argv[i]);
        return 0;
}

