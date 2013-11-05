#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

#include "remove.h"
#include "global.h"

#define OPT_REMOVE_FORCE 0x1
#define OPT_REMOVE_CONFIRM 0x2

static struct option longopts[] = {
        {"help", no_argument, NULL, 'h'},
        {"force", no_argument, NULL, 'f'},
        {"confirm", no_argument, NULL, 'c'},
        {NULL, 0, NULL, 0}
};

const char rss_remove_usage_string[] = 
        "Remove a feed and associated data from the database.\n\n"
        "Usage: rss remove [<options>] feed\n"
        "where options are:\n"
        "   -h, --help          print this help message and exit\n"
        "   -f, --force         remove feed data without asking\n"
        "   -i, --interactive   request confirmation before removing a non-empty directory\n";


static void usage(void) {
        printf("%s\n", rss_remove_usage_string);
}

static int parse_args(int argc, char **argv, int *options) {
        int ch;
        *options = 0;
        while ((ch = getopt_long(argc, argv, "hfi", longopts, NULL)) != -1) {
                switch (ch) {
                case 'f':
                        if (*options & OPT_REMOVE_CONFIRM) {
                                fprintf(stderr, "error: you can't specify both -f and -i.\n");
                                return -1;
                        }
                        *options = OPT_REMOVE_FORCE;
                        break;
                case 'i':
                        if (*options & OPT_REMOVE_FORCE) {
                                fprintf(stderr, "error: you can't specify both -f and -i.\n");
                                return -1;
                        }
                        *options = OPT_REMOVE_CONFIRM;
                        break;
                case 'h':
                        usage();
                        exit(0);
                default:
                        fprintf(stderr, "fatal: %c: unknown option.\n", ch);
                        return -1;
                }
        }
        if (optind == argc) {
                fprintf(stderr, "You must provide the alias or the URL of at least one feed to remove.\n");
                return -1;
        }
        return 0;
}

/*
static int remove_line_from_file(const char *filename, int lineno) {
        FILE *fid, *tmp;
        ssize_t read;
        size_t len;
        char *line = NULL;
        int curr, retval = 0;

        if (lineno > 0) {
                fid = fopen(filename,"r");
                tmp = fopen("/tmp/rss.txt","w");
                while ((read = getline(&line, &len, fid)) != -1) {
                        if (strlen(line) && line[0] != '#')
                                curr++;
                        if (curr != lineno)
                                fprintf(tmp, "%s", line);
                }
                fclose(fid);
                fclose(tmp);
                if (curr >= lineno) {
                        fid = fopen(filename,"w");
                        tmp = fopen("/tmp/rss.txt","r");
                        while ((read = getline(&line, &len, tmp)) != -1)
                                fprintf(fid, "%s", line);
                        fclose(fid);
                        fclose(tmp);
                }
                else {
                        fprintf(stderr, "error: file %s contains less than %d lines.\n", filename, lineno);
                        retval = -1;
                }
        }
        if (line)
                free(line);
        return retval;
}
*/

static int remove_dir_recursively(char *dirname) {
        FTS *ftsp;
        FTSENT *p, *chp;
        int fts_options = FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR;
        
        if ((ftsp = fts_open(&dirname, fts_options, NULL)) == NULL) {
                perror("fts_open");
                return -1;
        }
        chp = fts_children(ftsp, 0);
        if (chp == NULL) {
                fts_close(ftsp);
                rmdir(dirname);
                return 0;
        }
        while ((p = fts_read(ftsp)) != NULL) {
                switch (p->fts_info) {
                case FTS_D:
                        if (strcmp(dirname, p->fts_path)) {
                                remove_dir_recursively(p->fts_path);
                                rmdir(p->fts_path);
                        }
                        break;
                case FTS_F:
                        unlink(p->fts_path);
                        break;
                default:
                        break;
                }
        }
        fts_close(ftsp);
        rmdir(dirname);
        return 0;
}

static int remove_feed(const char *feed, int options) {
        char url[URL_MAX], alias[PATH_MAX], path[PATH_MAX];
        int err; 
        char answer;

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

        if (options & OPT_REMOVE_CONFIRM) {
                printf("remove feed %s? (y/N) ", alias);
                scanf("%c", &answer);
                if (answer != 'y' && answer != 'Y') {
                        printf("not removing feed %s.\n", alias);
                        return 0;
                }
        }

        printf("removing feed %s.\n", alias);
        sprintf(path, "%s/%s", RSS_DIR, alias);

        // first remove the data
        if (options & OPT_REMOVE_FORCE) {
                err = remove_dir_recursively(path);
                if (err) {
                        fprintf(stderr, "error: unable to remove directory %s.\n", path);
                        return -1;
                }
        }
        else {
                err = rmdir(path);
                if (err) {
                        if (errno == ENOTEMPTY) {
                                fprintf(stderr, "%s: directory not empty: use -f to force removal.\n", path);
                                return -1;
                        }
                        else {
                                fprintf(stderr, "%s: unable to remove.\n", path);
                                return -1;
                        }
                }
        }

        rm_feed(url, alias);
        printf("successfully removed feed %s.\n", alias);
        return 0;
}

int cmd_remove(int argc, char **argv) {
        int options, i;
        if (parse_args(argc, argv, &options) != 0)
                return -1;
        for (i=optind; i<argc; i++) {
                if (remove_feed(argv[i], options) != 0)
                        return -1;
        }
        return 0;
}

