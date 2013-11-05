#ifndef NODE_H
#define NODE_H

#include "global.h"

struct _node {
        char alias[ALIAS_MAX];
        char url[URL_MAX];
        struct _node * next;
        struct _node * prev;
};

struct _list {
        struct _node * head;
        struct _node * tail;
        size_t n_elem;
};

typedef struct _node * node;
typedef struct _list * list;

node   node_create(const char *alias, const char *url);
void   node_destroy(node n);
char*  node_alias(node n);
char*  node_url(node n);

list   list_create(void);
void   list_destroy(list l);
size_t list_size(const list l);
int    list_empty(const list l);
int    list_push_back(list l, node n);
int    list_push_front(list l, node n);
node   list_pop_back(list l);
node   list_pop_front(list l);
node   list_front(const list l);
node   list_back(const list l);

void test_list(void);

/* parses the feeds file and returns all pairs (alias,url) in a linked list */
list parse_feeds_file(void);

#endif

