#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>

#define RSS_DIR             ".rss"
#define FEEDS_FILE          "feeds"
#define NONE                0x0
#define NEED_RSS_DIR        0x1
#ifndef PATH_MAX
#define PATH_MAX            4096
#endif // PATH_MAX
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
#endif // getline

#include "node.h"

struct cmd_struct {
	char *cmd;
	int (*fn)(int, char **);
	int option;
};

void trim_newline(char *str);
int prefixcmp(const char *str, const char *prefix);
int suffixcmp(const char *str, const char *suffix);

/* splits a line of the feeds file in alias and URL */
void split_feeds_line(const char *line, char *alias, char *url);
/* generates an alias from a URL */
int url_to_alias(const char *url, char *alias);
/* finds a feed, given its URL */
int find_feed_alias(const char *url, char *alias);
/* finds a feed, given its alias */
int find_feed_url(const char *alias, char *url);
/* finds a feed, given both the URL and the alias */
int find_feed(const char *url, const char *alias);
/* removes a feed, given both the URL and the alias */
int rm_feed(const char *url, const char *alias);
/* adds a feed to the database */
int add_feed(const char *url, const char *alias);

/* copies a file from from to to */
int cp(const char *to, const char *from);

#endif // GLOBAL_H

