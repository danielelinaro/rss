#include "parse.h"
#include "global.h"
#include "fetch.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <libxml/xmlreader.h>
#include <sys/stat.h>

static void parse_item(xmlNode *item_node, const char *feed_name) {
        xmlNode *cur_node = NULL, *date_node, *link_node;
        struct tm pub_date;
        char filename[15], path[PATH_MAX];
        struct stat buf;
        for (cur_node=item_node; cur_node; cur_node=cur_node->next) {
                if (cur_node->type == XML_ELEMENT_NODE) {
                        if (!xmlStrcmp(cur_node->name, (xmlChar*) "link")) {
                                link_node = cur_node;
                        }
                        else if (!xmlStrcmp(cur_node->name, (xmlChar*) "pubDate")) {
                                date_node = cur_node;
                        }
                }
        }
        strptime((char *) xmlNodeGetContent(date_node->children), "%a, %d %b %Y %H:%M:%S %z", &pub_date);
        strftime(filename, 14, "%Y%m%d%H%M%S", &pub_date);
        sprintf(path, "%s/%s/%s", RSS_DIR, feed_name, filename);
        if (stat(path, &buf) == -1) {
                fetch_url((char *) xmlNodeGetContent(link_node->children), path);
        }
        else {
                printf("%s: file exists.\n", path);
        }
}

static void parse_node(xmlNode *a_node, const char *feed_name) {
        xmlNode *cur_node = NULL;
        for (cur_node=a_node; cur_node; cur_node=cur_node->next) {
                if (cur_node->type == XML_ELEMENT_NODE) {
                        if (!xmlStrcmp(cur_node->name, (xmlChar*) "item"))
                                parse_item(cur_node->children, feed_name);
                }
                parse_node(cur_node->children, feed_name);
        }
}

int parse_xml(const char *filename, const char *feed_name) {
        xmlDoc *doc = NULL;
        int err = 0;
        
        /*
         * this initialize the library and check potential ABI mismatches
         * between the version it was compiled for and the actual shared
         * library used.
         */
        LIBXML_TEST_VERSION
        
        /* parse the file and get the DOM */
        doc = xmlReadFile(filename, NULL, 0);
        
        if (doc != NULL) {
                parse_node(xmlDocGetRootElement(doc), feed_name);
                xmlFreeDoc(doc);
        }
        else {
            printf("error: could not parse file %s\n", filename);
            err = -1;
        }
        
        xmlCleanupParser();
        return err;
}

