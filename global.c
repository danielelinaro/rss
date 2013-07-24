#include <stdio.h>
#include <string.h>

#include "global.h"

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
                if (line[0] == '#')
                        continue;
                line[read] = '\0';
                if (strncmp(line, text, MAX(strlen(line),text_len)) == 0)
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

