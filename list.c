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


static void usage() {
        printf("%s\n", rss_list_usage_string);
}

static int parse_args(int argc, char **argv) {
        int ch;
        while ((ch = getopt_long(argc, argv, "h", longopts, NULL)) != -1) {
                switch (ch) {
                case 'h':
                        usage();
                        return -1;
                default:
                        fprintf(stderr, "fatal: %c: unknown option.\n", ch);
                        return -1;
                }
        }
        return 0;
}

int cmd_list(int argc, char **argv) {
        FILE *feeds_fid, *aliases_fid;
        char path[PATH_MAX], *feeds_line, *aliases_line;
        size_t len[2] = {0,0};
        ssize_t read[2] = {0,0};
        int retval = 0;

        if (parse_args(argc,argv))
                return -1;

        if (check_feeds_integrity() != 0) {
                fprintf(stderr, "fatal: the feeds file is corrupted.\n");
                return -1;
        }

        sprintf(path, "%s/%s", RSS_DIR, FEEDS_FILE);
        feeds_fid = fopen(path,"r");
        if (feeds_fid == NULL) {
                fprintf(stderr, "fatal: unable to read %s.\n", FEEDS_FILE);
                return -1;
        }
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_ALIASES_FILE);
        aliases_fid = fopen(path,"r");
        if (aliases_fid == NULL) {
                fprintf(stderr, "fatal: unable to read %s.\n", FEEDS_ALIASES_FILE);
                fclose(feeds_fid);
                return -1;
        }

        printf("The following feeds are currently in the database:\n");
        while ((read[0] = getline(&feeds_line, &len[0], feeds_fid)) != -1) {
                if (strlen(feeds_line) == 0 || feeds_line[0] == '#')
                        continue;
                while ((read[1] = getline(&aliases_line, &len[1], aliases_fid)) != -1)
                        if (strlen(aliases_line) != 0 && aliases_line[0] != '#')
                                break;
                feeds_line[read[0]] = '\0';
                aliases_line[read[1]] = '\0';
                printf("%s - %s\n", aliases_line, feeds_line);
        }
 
        if (feeds_line)
                free(feeds_line);
        if (aliases_line)
                free(aliases_line);
        fclose(feeds_fid);
        fclose(aliases_fid);
        return retval;
}

