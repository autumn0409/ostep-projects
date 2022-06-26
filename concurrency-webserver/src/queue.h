#ifndef QUEUE_H
#define QUEUE_H

#include "node.h"

typedef struct queue_t queue_t;

#ifdef __cplusplus
extern "C" {
#endif

struct queue_t {
    int count;
    int queue_size;
    node_t *head;
    node_t *tail;
};

queue_t *create_new_queue(int queue_size);
void delete_queue(queue_t *self);
int enqueue(queue_t *self, node_t *new_node);
node_t *dequeue(queue_t *self);

#ifdef __cplusplus
}
#endif

#endif /* QUEUE_H */