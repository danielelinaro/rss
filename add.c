#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <getopt.h>
#include <string.h>

#include "add.h"
#include "global.h"

char feed_url[1024];
char feed_alias[1024];

static struct option longopts[] = {
        {"help", no_argument, NULL, 'h'},
        {"alias", required_argument, NULL, 'a'},
        {NULL, 0, NULL, 0}
};

const char rss_add_usage_string[] = 
        "Add the address of an RSS feed to the list of feeds.\n\n"
        "Usage: rss add [<options> ...] feed\n"
        "where options are:\n"
        "   -h, --help      print this help message and exit\n"
        "   -a, --alias     provide an alias for the address of the feed\n";


void usage() {
        printf("%s\n", rss_add_usage_string);
}

int parse_args(int argc, char **argv) {
        int ch, i;
        feed_alias[0] = '\0';
        while ((ch = getopt_long(argc, argv, "ha:", longopts, NULL)) != -1) {
                switch (ch) {
                case 'a':
                        strncpy(feed_alias, optarg, 1024);
                        break;
                case 'h':
                        usage();
                        return -1;
                default:
                        fprintf(stderr, "fatal: %c: unknown option.\n", ch);
                        return -1;
                }
        }
        if (optind != argc-1) {
                fprintf(stderr, "fatal: you must provide exactly one URL of a feed.\n");
                return -1;
        }
        strncpy(feed_url,argv[optind],1024);
        if (!strlen(feed_alias))
                strncpy(feed_alias,argv[optind],1024);
        return 0;
}

int cmd_add(int argc, char **argv) {
        struct stat buf;
        char urls[PATH_MAX], aliases[PATH_MAX];

        if (stat(RSS_DIR, &buf) == -1) {
                if (errno == ENOENT)
                        fprintf(stderr, "fatal: %s: no such directory.\n"
                                        "did you initialise this directory with 'rss init'?\n", RSS_DIR);
                else
                        perror("rss add:");
                return -1;
        }

        if (parse_args(argc, argv) != 0)
                return -1;

        sprintf(urls, "%s/%s", RSS_DIR, FEEDS_FILE);
        sprintf(aliases, "%s/%s", RSS_DIR, FEEDS_ALIASES_FILE);

        if (check_in_file(urls,feed_url) > 0) {
                fprintf(stderr, "fatal: URL %s already present.\n", feed_url);
                return -1;
        }

        if (check_in_file(aliases,feed_alias) > 0) {
                fprintf(stderr, "fatal: alias %s already present.\n", feed_alias);
                return -1;
        }

        return ! (!append_to_file(urls,feed_url) && !append_to_file(aliases,feed_alias));
}

