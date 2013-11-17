#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <sys/stat.h>
#include <string.h>

#include "add.h"
#include "fetch.h"
#include "global.h"
#include "parse.h"

char feed_url[URL_MAX];
char feed_alias[URL_MAX];

static struct option longopts[] = {
        {"help", no_argument, NULL, 'h'},
        {"alias", required_argument, NULL, 'a'},
        {"fetch", no_argument, NULL, 'f'},
        {NULL, 0, NULL, 0}
};

const char rss_add_usage_string[] = 
        "Add the address of an RSS feed to the list of feeds.\n\n"
        "Usage: rss add [<options> ...] feed\n"
        "where options are:\n"
        "   -h, --help      print this help message and exit\n"
        "   -a, --alias     provide an alias for the address of the feed\n"
        "   -f, --fetch     fetch the data\n";

static void usage(void) {
        printf("%s\n", rss_add_usage_string);
}

static int parse_args(int argc, char **argv, int *options) {
        int ch;
        char answer, *ptr;
        feed_alias[0] = '\0';
        ptr = feed_alias;
        *options = 0x0;
        while ((ch = getopt_long(argc, argv, "ha:f", longopts, NULL)) != -1) {
                switch (ch) {
                case 'a':
                        strcpy(feed_alias, optarg);
                        break;
                case 'f':
                        *options |= RSS_ADD_FETCH;
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
                fprintf(stderr, "fatal: you must provide exactly one URL of a feed.\n");
                return -1;
        }
        strcpy(feed_url,argv[optind]);
        if (!strlen(feed_alias)) {
                url_to_alias(feed_url, feed_alias);
                printf("Will use [%s] as an alias for this feed. Is it ok? (Y/n) ", feed_alias);
                scanf("%c", &answer);
                if (answer == 'n' || answer == 'N') {
                        printf("Enter the name of the alias: ");
                        scanf("%s", ptr);
                }
        }
        return 0;
}

int cmd_add(int argc, char **argv) {
        int options;
        char path[PATH_MAX];

        if (parse_args(argc, argv, &options) != 0)
                return -1;

        if (add_feed(feed_url, feed_alias) != 0) {
                fprintf(stderr, "Unable to add the feed to the database.\n");
                return -1;
        }

        sprintf(path, "%s/%s", RSS_DIR, feed_alias);
        mkdir(path, 0755);

        if (options & RSS_ADD_FETCH) {
                sprintf(path, "%s/%s/feed.xml", RSS_DIR, feed_alias);
                fetch_url(feed_url, path);
                parse_xml(path, feed_alias);
        }

        return 0;
}

