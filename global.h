#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>

#define RSS_DIR             ".rss"
#define FEEDS_FILE          "feeds"
#define FEEDS_ALIASES_FILE  "feeds_aliases"
#define NONE                0
#ifndef PATH_MAX
#define PATH_MAX            4096
#endif

#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))

#ifndef getline
#include <stdlib.h>  /* malloc, realloc */
#include <errno.h>   /* errno */
#include <unistd.h>  /* ssize_t */
#define MY_GETLINE
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
#endif

int append_to_file(const char *filename, const char *text);
int check_in_file(const char *filename, const char *text);
int check_feeds_integrity(void);

#endif

