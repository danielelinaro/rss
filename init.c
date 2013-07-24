#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

#include "init.h"
#include "global.h"

int cmd_init(int argc, char **argv) {
        FILE *fid;
        char path[PATH_MAX];
        int err;
        err = mkdir(RSS_DIR, 0755);
        if (err) {
                if (errno == EEXIST)
                        fprintf(stderr, "fatal: %s: directory exists.\n", RSS_DIR);
                else
                        perror("rss init:");
                return -1;
        }
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_FILE);
        fid = fopen(path, "w");
        if (fid == NULL) {
                fprintf(stderr, "fatal: unable to create feeds file.\n");
                return -1;
        }
        fprintf(fid, "# this file contains the addresses of the RSS feeds\n");
        fclose(fid);
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_ALIASES_FILE);
        fid = fopen(path, "w");
        if (fid == NULL) {
                fprintf(stderr, "fatal: unable to create feeds aliases file.\n");
                return -1;
        }
        fprintf(fid, "# this file contains the aliases of the RSS feeds\n");
        fclose(fid);
        return 0;
}

