#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <sys/stat.h>
#include <string.h>

#include "add.h"
#include "fetch.h"
#include "global.h"

char feed_url[1024];
char feed_alias[1024];

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
        char answer;
        feed_alias[0] = '\0';
        options = 0x0;
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
                        scanf("%s", &feed_alias);
                }
        }
        return 0;
}

int cmd_add(int argc, char **argv) {
        int options, i;
        char urls[PATH_MAX], aliases[PATH_MAX], path[PATH_MAX];

        if (parse_args(argc, argv, &options) != 0)
                return -1;

        sprintf(urls, "%s/%s", RSS_DIR, FEEDS_FILE);
        sprintf(aliases, "%s/%s", RSS_DIR, FEEDS_ALIASES_FILE);

        if (check_in_file(urls,feed_url) == 0) {
                fprintf(stderr, "fatal: URL %s already present.\n", feed_url);
                return -1;
        }

        if (check_in_file(aliases,feed_alias) == 0) {
                fprintf(stderr, "fatal: alias %s already present.\n", feed_alias);
                return -1;
        }

        for (i=0; i<strlen(feed_alias); i++)
                if (feed_alias[i] == '/' || feed_alias[i] == ':' || feed_alias[i] == '.')
                        feed_alias[i] = '_';

        append_to_file(urls,feed_url);
        append_to_file(aliases,feed_alias);

        sprintf(path, "%s/%s", RSS_DIR, feed_alias);
        mkdir(path, 0755);

        if (options & RSS_ADD_FETCH)
                fetch_url(feed_url, path);

        return 0;
}

