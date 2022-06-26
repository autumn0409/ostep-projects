#include "node.h"

#include <stdlib.h>

node_t *create_new_node(int fd) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));

    if (!new_node)
        return new_node;

    new_node->fd = fd;
    new_node->next = NULL;
    new_node->prev = NULL;

    return new_node;
}
