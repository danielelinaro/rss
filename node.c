#include "node.h"
#include <string.h>

node node_create(const char *alias, const char *url) {
        node n;
        n = (node) malloc(sizeof(struct _node));
        if (n != NULL) {
                if (alias)
                        strcpy(n->alias,alias);
                else
                        n->alias[0] = '\0';
                if (url)
                        strcpy(n->url,url);
                else
                        n->url[0] = '\0';
                n->next = n->prev = NULL;
                //printf("node_create: (%s,%s)\n", alias, url);
        }
        else {
                fprintf(stderr, "node_create: unable to allocate node.\n");
        }
        return n;
}

void node_destroy(node n) {
        if (n) {
                //printf("node_destroy: (%s,%s)\n", node_alias(n), node_url(n));
                free(n);
        }
        else {
                fprintf(stderr, "node_destroy: n is NULL.\n");
        }
}

char* node_alias(node n) {
        if (n)
                return n->alias;
        fprintf(stderr, "node_alias: n is NULL.\n");
        return NULL;
}

char* node_url(node n) {
        if (n)
                return n->url;
        fprintf(stderr, "node_url: n is NULL.\n");
        return NULL;
}

list list_create(void) {
        list l;
        l = (list) malloc(sizeof(struct _list));
        if (l != NULL) {
                l->head = NULL;
                l->tail = NULL;
                l->n_elem = 0;
        }
        else {
                fprintf(stderr, "list_create: unable to allocate list.\n");
        }
        return l;
}

void list_destroy(list l) {
        if (l) {
                if (!list_empty(l)) {
                        node n = l->head;
                        while (n->next) {
                                n = n->next;
                                node_destroy(n->prev);
                        }
                        node_destroy(n);
                }
                free(l);
        }
        else {
                fprintf(stderr, "list_destroy: l is NULL.\n");
        }
}

size_t list_size(const list l) {
        if (l)
                return l->n_elem;
        fprintf(stderr, "list_size: l is NULL.\n");
        return 0;
}

int list_empty(const list l) {
        return list_size(l) == 0;
}

int list_push_front(list l, node n) {
        if (l == NULL || n == NULL)
                return -1;
        if (!list_empty(l)) {
                // there are already nodes in the list
                n->next = l->head;
                n->prev = NULL;
                l->head->prev = n;
                l->head = n;
        }
        else {
                // the list is empty
                l->head = n;
                l->tail = n;
        }
        l->n_elem++;
        return 0;
}

int list_push_back(list l, node n) {
        if (l == NULL || n == NULL)
                return -1;
        if (!list_empty(l)) {
                // there are already nodes in the list
                n->prev = l->tail;
                n->next = NULL;
                l->tail->next = n;
                l->tail = n;
        }
        else {
                // the list is empty
                l->head = n;
                l->tail = n;
        }
        l->n_elem++;
        return 0;
}

node list_pop_front(list l) {
        node n = NULL;
        if (!list_empty(l)) {
                l->n_elem--;
                n = l->head;
                l->head = l->head->next;
                if (!list_empty(l))
                        l->head->prev = NULL;
                else
                        l->tail = NULL;
        }
        return n;
}

node list_pop_back(list l) {
        node n = NULL;
        if (!list_empty(l)) {
                l->n_elem--;
                n = l->tail;
                l->tail = l->tail->prev;
                if (!list_empty(l))
                        l->tail->next = NULL;
                else
                        l->head = NULL;
        }
        return n;
}

node list_front(const list l) {
        if (l)
                return l->head;
        fprintf(stderr, "list_front: l is NULL.\n");
        return NULL;
}

node list_back(const list l) {
        if (l)
                return l->tail;
        fprintf(stderr, "list_back: l is NULL.\n");
        return NULL;
}

list parse_feeds_file(void) {
        char path[PATH_MAX], alias[URL_MAX], url[URL_MAX], *line;
        FILE *fid;
        size_t len;
        list l = NULL;

        sprintf(path, "%s/%s", RSS_DIR, FEEDS_FILE);
        fid = fopen(path,"r");
        if (fid != NULL) {
                while ((line = fgetln(fid, &len)) != NULL) {
                        line[len-1] = '\0';
                        split_feeds_line(line, alias, url);
                        if (strlen(alias) && strlen(url)) {
                                if (!l) {
                                        l = list_create();
                                        if (!l)
                                                break;
                                }
                                list_push_back(l, node_create(alias, url));
                        }
                }
                fclose(fid);
        }
        return l;
}

void test_list(void) {
        list l;
        node n, m;
        l = list_create();
        printf("list size: %ld\n", list_size(l));
        n = node_create("foo", "foo.com");
        list_push_back(l,n);
        printf("list size: %ld\n", list_size(l));
        n = node_create("bar", "bar.com");
        list_push_back(l,n);
        printf("list size: %ld\n", list_size(l));
        n = node_create("spam", "spam.com");
        list_push_front(l,n);
        printf("list size: %ld\n", list_size(l));
        m = list_pop_front(l);
        printf("(%s,%s)\n", node_alias(m), node_url(m));
        printf("list size: %ld\n", list_size(l));
        list_destroy(l);
        node_destroy(m);
}

