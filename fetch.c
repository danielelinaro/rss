#include <getopt.h>
#include <string.h>
#include <curl/curl.h>

#include "fetch.h"
#include "global.h"

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


char feed[1024];
int fetch_all;

static void usage() {
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
                        return -1;
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
                        strncpy(feed,argv[optind],1024);
                }
        }
        return 0;
}


int cmd_fetch(int argc, char **argv) {
        int retval = 0;
        CURL *curl;
        CURLcode res;
 
        parse_args(argc, argv);

        curl = curl_easy_init();
        if (!curl) {
                fprintf(stderr, "fatal: unable to initialise libcurl.\n");
                return -1;
        }

        curl_easy_setopt(curl, CURLOPT_URL, feed);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
                fprintf(stderr, "fatal: %s.\n", curl_easy_strerror(res));
                retval = -1; 
        }
        curl_easy_cleanup(curl);
        
        return retval;
}

