#include <stdio.h>
#include <string.h>

#include "global.h"

int prefixcmp(const char *str, const char *prefix) {
        for (; ; str++, prefix++)
                if (!*prefix)
                        return 0;
                else if (*str != *prefix)
                        return (unsigned char)*prefix - (unsigned char)*str;
}

int suffixcmp(const char *str, const char *suffix)
{
        int len = strlen(str), suflen = strlen(suffix);
        if (len < suflen)
                return -1;
        else
                return strcmp(str + len - suflen, suffix);
}

int append_to_file(const char *filename, const char *text) {
        FILE *fid;
        fid = fopen(filename,"a");
        if (fid == NULL) {
                fprintf(stderr, "fatal: unable to open %s for writing.\n", filename);
                return -1;
        }
        fprintf(fid, "%s\n", text);
        fclose(fid);
        return 0;
}

int check_in_file(const char *filename, const char *text) {
        FILE *fid;
        int lineno = 0;
        char *line = NULL;
        size_t len, text_len;
        ssize_t read;
        text_len = strlen(text);
        fid = fopen(filename,"r");
        if (fid == NULL) {
                fprintf(stderr, "fatal: unable to open %s for reading.\n", filename);
                return -1;
        }
        while ((read = getline(&line, &len, fid)) != -1) {
                lineno++;
                if (strlen(line) || line[0] == '#')
                        continue;
                while (line[read] != '\n')
                        read--;
                line[read] = '\0';
                if (strcmp(line, text) == 0)
                        break;
        }
        fclose(fid);
        if (line)
                free(line);
        return (read == -1 ? 0 : lineno);
}

int check_feeds_integrity(void) {
        FILE *fid;
        char *line, path[PATH_MAX];
        ssize_t read;
        size_t len, lineno = 0;
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_FILE);
        fid = fopen(path, "r");
        if (fid == NULL) {
                fprintf(stderr, "fatal: unable to open %s\n", FEEDS_FILE);
                return -1;
        }
        while ((read = getline(&line, &len, fid)) != -1)
                if (strlen(line) && line[0] != '#')
                        lineno++;
        fclose(fid);
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_ALIASES_FILE);
        fid = fopen(path, "r");
        if (fid == NULL) {
                fprintf(stderr, "fatal: unable to open %s\n", FEEDS_ALIASES_FILE);
                if (line)
                        free(line);
                return -1;
        }
        while ((read = getline(&line, &len, fid)) != -1)
                if (strlen(line) && line[0] != '#')
                        lineno--;
        fclose(fid);
        if (line)
                free(line);
        return lineno;
}

int url_to_alias(const char *url, char *alias) {
        char *input, *ptr;
        char **ap, *argv[100];
        int argc;
        input = malloc(URL_MAX * sizeof(char));
        strcpy(input, url);

        for (ap = argv, argc=0; (*ap = strsep(&input, ".")) != NULL; argc++)
                if (**ap != '\0')
                        if (++ap >= &argv[100])
                                break;
        
        if (!suffixcmp(argv[0],"www")) {
                strcpy(alias, argv[1]);
        }
        else {
                for (ptr=argv[0]; ptr!=argv[1]; ptr++) {
                        if (*ptr == '/') {
                                ptr += 2;
                                break;
                        }
                }
                if (ptr==argv[1])
                        ptr = argv[0];
                strcpy(alias, ptr);
        }
        free(input);
        return 0;
}

static int find_line(const char *filename, const char *text) {
        FILE *fid;
        char *line = NULL;
        int lineno = 1;
        ssize_t read;
        size_t len, text_len = strlen(text);
        fid = fopen(filename,"r");
        if (fid == NULL)
                return -1;
        while ((read = getline(&line, &len, fid)) != -1) {
                if (!strlen(line) || line[0] == '#')
                        continue;
                while (line[read] != '\n')
                        read--;
                line[read] = '\0';
                if (strlen(line) == text_len && strcmp(line, text) == 0)
                        break;
                lineno++;
        }
        if (line)
                free(line);
        fclose(fid);
        return (read == -1 ? -1 : lineno);
}

static int extract_line(const char *filename, int lineno, char *text) {
        FILE *fid;
        char *line = NULL;
        int curr = 1;
        ssize_t read;
        size_t len;
        fid = fopen(filename,"r");
        if (fid == NULL)
                return -1;
        while ((read = getline(&line, &len, fid)) != -1) {
                if (!strlen(line) || line[0] == '#')
                        continue;
                while (line[read] != '\n')
                        read--;
                line[read] = '\0';
                if (curr == lineno) {
                        strcpy(text, line);
                        break;
                }
                curr++;
        }
        if (line)
                free(line);
        fclose(fid);
        return (read == -1 ? -1 : 0);
}

int find_feed_url(const char *feed, char *url, char *alias) {
        int lineno;
        char path[PATH_MAX];
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_ALIASES_FILE);
        lineno = find_line(path, feed);
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_FILE);
        if (lineno > 0) {
                if (extract_line(path, lineno, url) != 0) {
                        fprintf(stderr, "error: unable to extract line %d from file %s.\n", lineno, path);
                        return -1;
                }
                strcpy(alias, feed);
                return lineno;
        }
        lineno = find_line(path, feed);
        if (lineno > 0) {
                if (extract_line(path, lineno, alias) != 0) {
                        fprintf(stderr, "error: unable to extract line %d from file %s.\n", lineno, path);
                        return -1;
                }
                strcpy(url, feed);
                return lineno;
        }
        fprintf(stderr, "error: unable to find feed %s\n", feed);
        return -1;
}

#ifdef MY_GETLINE
ssize_t getline(char **linep, size_t *np, FILE *stream)
{
  char *p = NULL;
  size_t i = 0;
  int ch;

  if (!linep || !np) {
    errno = EINVAL;
    return -1;
  }

  if (!(*linep) || !(*np)) {
    *np = 120;
    *linep = (char *)malloc(*np);
    if (!(*linep)) {
      return -1;
    }
  }

  flockfile(stream);

  p = *linep;
  for (ch = 0; (ch = getc_unlocked(stream)) != EOF;) {
    if (i > *np) {
      /* Grow *linep. */
      size_t m = *np * 2;
      char *s = (char *)realloc(*linep, m);

      if (!s) {
        int error = errno;
        funlockfile(stream);
        errno = error;
        return -1;
      }

      *linep = s;
      *np = m;
    }

    p[i] = ch;
    if ('\n' == ch) break;
    i += 1;
  }
  funlockfile(stream);

  /* Null-terminate the string. */
  if (i > *np) {
    /* Grow *linep. */
    size_t m = *np * 2;
    char *s = (char *)realloc(*linep, m);

    if (!s) {
      return -1;
    }

    *linep = s;
    *np = m;
  }

  p[i + 1] = '\0';
  return ((i > 0)? i : -1);
}
#endif // MY_GETLINE

