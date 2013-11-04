#include <getopt.h>
#include <string.h>
#include <curl/curl.h>

#include "fetch.h"
#include "global.h"
#include "parse.h"

static struct option longopts[] = {
        {"help", no_argument, NULL, 'h'},
        {"all", no_argument, NULL, 'a'},
        {NULL, 0, NULL, 0}
};

const char rss_fetch_usage_string[] = 
        "Fetch all or a specified feed.\n\n"
        "Usage: rss fetch [<options> ...] feed\n"
        "where options are:\n"
        "   -h, --help      print this help message and exit\n"
        "   -a, --all       fetch all feeds\n";


char feed[URL_MAX];
int fetch_all;

static void usage(void) {
        printf("%s\n", rss_fetch_usage_string);
}

static int parse_args(int argc, char **argv) {
        int ch;
        fetch_all = 0;
        feed[0] = '\0';
        while ((ch = getopt_long(argc, argv, "ha", longopts, NULL)) != -1) {
                switch (ch) {
                case 'a':
                        fetch_all = 1;
                        break;
                case 'h':
                        usage();
                        exit(0);
                default:
                        fprintf(stderr, "fatal: %c: unknown option.\n", ch);
                        return -1;
                }
        }
        if (fetch_all && optind != argc) {
                fprintf(stderr, "fatal: if -a is set, no feed URL or alias should be specified.\n");
                return -1;
        }
        if (!fetch_all) {
                if (optind != argc-1) {
                        fprintf(stderr, "fatal: you must provide exactly one URL or alias of a feed.\n");
                        return -1;
                }
                else {
                        strcpy(feed,argv[optind]);
                }
        }
        return 0;
}

int fetch_url(const char *url, const char *filename) {
        int retval = 0;
        FILE *fid;
        CURL *curl;
        CURLcode res;
        fid = fopen(filename, "w");
        if (fid == NULL) {
                fprintf(stderr, "fatal: unable to open %s for writing.\n", filename);
                return -1;
        }
        printf("Fetching %s to %s.\n", url, filename);
        curl = curl_easy_init();
        if (!curl) {
                fprintf(stderr, "fatal: unable to initialise libcurl.\n");
                fclose(fid);
                return -1;
        }
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fid);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
                fprintf(stderr, "fatal: %s.\n", curl_easy_strerror(res));
                retval = -1; 
        }
        curl_easy_cleanup(curl);
        fclose(fid);
        return retval;
}

int cmd_fetch(int argc, char **argv) {
        FILE *fid;
        ssize_t read;
        size_t len;
        char url[URL_MAX], alias[PATH_MAX], path[PATH_MAX], *line;
        int err;
 
        parse_args(argc, argv);

        if (fetch_all) {
                sprintf(path, "%s/%s", RSS_DIR, FEEDS_ALIASES_FILE);
                fid = fopen(path,"r");
                while ((read = getline(&line, &len, fid)) != -1) {
                        split_feeds_line(line, alias, url);
                        if (alias[0] && url[0]) {
                                sprintf(path, "%s/%s/feed.xml", RSS_DIR, alias);
                                printf("Fetching %s to %s.\n", url, path);
                                err = fetch_url(url, path);
                                if (err) {
                                        fprintf(stderr, "Unable to fetch %s\n", alias);
                                }
                                else {
                                        printf("Successfully fetched %s\n", alias);
                                        err = parse_xml(path);
                                        if (err)
                                                fprintf(stderr, "Unable to parse %s\n", path);
                                        else
                                                printf("Successfully parsed %s\n", path);
                                }

                        }
                }
                if (line)
                        free(line);
                fclose(fid);
        }
        else {
                if (find_feed_url(feed, url)) {
                        // the user passed the alias
                        strcpy(alias, feed);
                }
                else {
                        if (find_feed_alias(feed, alias) > 0) {
                                // the user passed the url
                                strcpy(url, feed);
                        }
                        else {
                                // can't find the feed
                                fprintf(stderr, "fatal: cannot find feed %s\n", feed);
                                return -1;
                        }
                }
                sprintf(path, "%s/%s/feed.xml", RSS_DIR, alias);
                printf("Fetching %s to %s.\n", url, path);
                err = fetch_url(url, path);
                if (err) {
                        fprintf(stderr, "Unable to fetch %s\n", alias);
                        return -1;
                }
                else {
                        printf("Successfully fetched %s\n", alias);
                        err = parse_xml(path);
                        if (err) {
                                fprintf(stderr, "Unable to parse %s\n", path);
                                return -1;
                        }
                        else {
                                printf("Successfully parsed %s\n", path);
                        }
                }
        }

        return 0;
}

