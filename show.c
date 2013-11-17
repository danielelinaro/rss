#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include "show.h"
#include "global.h"
#include "fetch.h"
#include "parse.h"

char feed[URL_MAX];

static struct option longopts[] = {
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
};

const char rss_show_usage_string[] = 
        "Show a feed.\n\n"
        "Usage: rss show [<options> ...] feed\n"
        "where options are:\n"
        "   -h, --help      print this help message and exit\n"
        "   -f, --fetch     fetch the feed data before showing it\n";


static void usage(void) {
        printf("%s\n", rss_show_usage_string);
}

static int parse_args(int argc, char **argv, int *options) {
        int ch;
        *options = 0;
        while ((ch = getopt_long(argc, argv, "fh", longopts, NULL)) != -1) {
                switch (ch) {
                case 'f':
                        *options |= RSS_SHOW_FETCH;
                        break;
                case 'h':
                        usage();
                        exit(0);
                default:
                        fprintf(stderr, "fatal: %c: unknown option.\n", ch);
                        return -1;
                }
        }
        if (optind != argc-1) {
                fprintf(stderr, "fatal: you must provide exactly one URL or alias of a feed.\n");
                return -1;
        }
        else {
                strcpy(feed,argv[optind]);
        }
        return 0;
}

int show_feed(const char *filename) {
        fprintf(stderr, "not implemented yet.\n");
        return 0;
}

int cmd_show(int argc, char **argv) {
        int options;
        char url[URL_MAX], alias[ALIAS_MAX], filename[PATH_MAX];

        if (parse_args(argc, argv, &options) != 0)
                return -1;

        if (find_feed_url(feed, url)) {
                strcpy(alias, feed);
        }
        else if (find_feed_alias(feed, alias)) {
                strcpy(url, feed);
        }
        else {
                fprintf(stderr, "%s: no such feed.\n", feed);
                return -1;
        }

        sprintf(filename, "%s/%s/feed.xml", RSS_DIR, alias);
        if (options & RSS_SHOW_FETCH)
                fetch_url(url, filename);

        if (show_feed(filename) != 0) {
                fprintf(stderr, "Unable to show the feed [%s].\n", alias);
                return -1;
        }

        return 0;
}

