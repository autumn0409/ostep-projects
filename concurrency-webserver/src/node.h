#ifndef NODE_H
#define NODE_H

#include <sys/stat.h>

typedef struct node_t node_t;

#define MAXBUF (8192)

#ifdef __cplusplus
extern "C" {
#endif

struct node_t {
    int fd;
    node_t *prev;
    node_t *next;

    char method[MAXBUF];
    char uri[MAXBUF];
    char version[MAXBUF];
    char filename[MAXBUF];
    char cgiargs[MAXBUF];
    int is_static;
    mode_t file_st_mode;
    off_t file_st_size;
};

node_t *create_new_node(int fd);

#ifdef __cplusplus
}
#endif

#endif /* NODE_H */