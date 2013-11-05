#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "global.h"
#include "node.h"

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

void trim_newline(char *str) {
        int pos = strlen(str) - 1;
        while (pos >= 0 && str[pos--] != '\n')
                ;
        if (pos)
                str[pos+1] = '\0';
}

void split_feeds_line(const char *line, char *alias, char *url) {
        char *ptr;
        const char *line_ptr = line;
        alias[0] = url[0] = 0;
        while (*line_ptr == ' ')
                line_ptr++;
        if (*line_ptr == '#')
                return;         // comment
        while (*line_ptr == '[')
                line_ptr++;
        ptr = alias;
        while (*line_ptr != ']') {
                *ptr = *line_ptr;
                line_ptr++;
                ptr++;
        }
        *ptr = '\0';
        while (*line_ptr == ']' || *line_ptr == ' ')
                line_ptr++;
        ptr = url;
        while (*line_ptr && *line_ptr != '\n') {
                *ptr = *line_ptr;
                line_ptr++;
                ptr++;
        }
        *ptr = '\0';
}

int find_feed_alias(const char *url, char *alias) {
        char path[PATH_MAX], url_l[URL_MAX], *line;
        FILE *fid;
        size_t len;
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_FILE);
        fid = fopen(path,"r");
        if (fid == NULL)
                return -1;
        while ((line = fgetln(fid, &len)) != NULL) {
                line[len-1] = '\0';
                split_feeds_line(line, alias, url_l);
                if (!strcmp(url, url_l))
                        break;
        }
        fclose(fid);
        if (line == NULL) {
                alias[0] = '\0';
                return 0;
        }
        return 1;
}

int find_feed_url(const char *alias, char *url) {
        char path[PATH_MAX], alias_l[URL_MAX], *line;
        FILE *fid;
        size_t len;
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_FILE);
        fid = fopen(path,"r");
        if (fid == NULL)
                return -1;
        while ((line = fgetln(fid, &len)) != NULL) {
                line[len-1] = '\0';
                split_feeds_line(line, alias_l, url);
                if (!strcmp(alias, alias_l))
                        break;
        }
        fclose(fid);
        if (line == NULL) {
                url[0] = '\0';
                return 0;
        }
        return 1;
}

int find_feed(const char *url, const char *alias) {
        char path[PATH_MAX], alias_l[URL_MAX], url_l[URL_MAX], *line;
        FILE *fid;
        size_t len;
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_FILE);
        fid = fopen(path,"r");
        if (fid == NULL)
                return -1;
        while ((line = fgetln(fid, &len)) != NULL) {
                line[len-1] = '\0';
                split_feeds_line(line, alias_l, url_l);
                if (!strcmp(alias, alias_l) && !strcmp(url, url_l))
                        break;
        }
        fclose(fid);
        if (line == NULL)
                return 0;
        return 1;
}

int rm_feed(const char *url, const char *alias) {
        char path[PATH_MAX], alias_l[URL_MAX], url_l[URL_MAX], *line;
        FILE *fid, *fid_cpy;
        size_t len;
        int err = 1;
        sprintf(path, "%s/%s", RSS_DIR, FEEDS_FILE);
        fid = fopen(path,"r");
        if (fid == NULL)
                return -1;
        fid_cpy = fopen("/tmp/feeds","w");
        if (fid_cpy == NULL) {
                fclose(fid);
                return -1;
        }
        while ((line = fgetln(fid, &len)) != NULL) {
                line[len-1] = '\0';
                split_feeds_line(line, alias_l, url_l);
                if (!strcmp(alias, alias_l) && !strcmp(url, url_l)) {
                        err = 0;
                        continue;
                }
                fprintf(fid_cpy, "%s\n", line);
        }
        fclose(fid);
        fclose(fid_cpy);
        if (!err) {
                remove(path);
                cp(path, "/tmp/feeds");
        }
        remove("/tmp/feeds");
        return err;
}

int add_feed(const char *url, const char *alias) {
        FILE *fid;
        char path[PATH_MAX];
        if (! find_feed(url, alias)) {
                sprintf(path, "%s/%s", RSS_DIR, FEEDS_FILE);
                fid = fopen(path,"a");
                if (fid == NULL)
                        return -1;
                fprintf(fid, "[%s] %s\n", alias, url);
                fclose(fid);
                return 0;
        }
        fprintf(stderr, "Feed already present in the database.\n");
        return -1;
}

int cp(const char *to, const char *from)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
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

