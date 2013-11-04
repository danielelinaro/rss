#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <getopt.h>

#include "init.h"
#include "global.h"

static struct option longopts[] = {
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
};

const char rss_init_usage_string[] = 
        "Initialise the current directory with an rss database.\n\n"
        "Usage: rss init [option]\n"
        "where option is:\n"
        "   -h, --help      print this help message and exit\n";


static void usage(void) {
        printf("%s\n", rss_init_usage_string);
}

static int parse_args(int argc, char **argv) {
        int ch;
        while ((ch = getopt_long(argc, argv, "h", longopts, NULL)) != -1) {
                switch (ch) {
                case 'h':
                        usage();
                        exit(0);
                default:
                        fprintf(stderr, "fatal: %c: unknown option.\n", ch);
                        return -1;
                }
        }
        return 0;
}

int cmd_init(int argc, char **argv) {
        FILE *fid;
        char path[PATH_MAX];
        int err;
        parse_args(argc, argv);
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
        fprintf(fid, "# this file contains the aliases and the URLs of the RSS feeds\n");
        fclose(fid);
        /*
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_ALIASES_FILE);
        fid = fopen(path, "w");
        if (fid == NULL) {
                fprintf(stderr, "fatal: unable to create feeds aliases file.\n");
                return -1;
        }
        fprintf(fid, "# this file contains the aliases of the RSS feeds\n");
        fclose(fid);
        */
        return 0;
}

