#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>

#define RSS_DIR             ".rss"
#define FEEDS_FILE          "feeds"
#define FEEDS_ALIASES_FILE  "feeds_aliases"
#define NONE                0x0
#define NEED_RSS_DIR        0x1
#ifndef PATH_MAX
#define PATH_MAX            4096
#endif
#define URL_MAX             4096
#define ALIAS_MAX           1024

#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))

#ifndef getline
#include <stdlib.h>  /* malloc, realloc */
#include <errno.h>   /* errno */
#include <unistd.h>  /* ssize_t */
#define MY_GETLINE
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
#endif

struct cmd_struct {
	char *cmd;
	int (*fn)(int, char **);
	int option;
};

void trim_newline(char *str);
int prefixcmp(const char *str, const char *prefix);
int suffixcmp(const char *str, const char *suffix);
int append_to_file(const char *filename, const char *text);
int check_in_file(const char *filename, const char *text);
int check_feeds_integrity(void);
int find_feed_url(const char *feed, char *url, char *alias);
int url_to_alias(const char *url, char *alias);

#endif

