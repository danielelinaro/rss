#include <getopt.h>
#include <string.h>

#include "list.h"
#include "global.h"

static struct option longopts[] = {
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
};

const char rss_list_usage_string[] = 
        "Display the list of feeds.\n\n"
        "Usage: rss list [<options>]\n"
        "where options are:\n"
        "   -h, --help      print this help message and exit\n";


static void usage(void) {
        printf("%s\n", rss_list_usage_string);
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

int cmd_list(int argc, char **argv) {
        FILE *fid;
        char path[PATH_MAX], url[URL_MAX], alias[URL_MAX], *line = NULL;
        size_t len;
        ssize_t read;
        int retval = 0, first = 1;

        if (parse_args(argc,argv))
                return -1;

        /*
        if (check_feeds_integrity() != 0) {
                fprintf(stderr, "fatal: the feeds file is corrupted.\n");
                return -1;
        }
        */

        sprintf(path, "%s/%s", RSS_DIR, FEEDS_FILE);
        fid = fopen(path,"r");
        if (fid == NULL) {
                fprintf(stderr, "fatal: unable to read %s.\n", FEEDS_FILE);
                return -1;
        }

        while ((read = getline(&line, &len, fid)) != -1) {
                split_feeds_line(line, alias, url);
                if (alias[0] && url[0]) {
                        if (first) {
                                printf("The following feeds are currently in the database:\n");
                                first = 0;
                        }
                        printf("%s - %s\n", alias, url);
                }
        }
 
        if (line)
                free(line);
        fclose(fid);

        if (first)
                printf("There are no feeds in the database.\n");

        return retval;
}

