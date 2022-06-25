#include "queue.h"

#include <stdlib.h>
#include <assert.h>

typedef struct node_t node_t;

struct node_t {
    int fd;
    int priority;
    node_t *prev;
    node_t *next;
};

struct queue_t {
    int count;
    int queue_size;
    node_t *head;
    node_t *tail;
};

node_t *create_new_node(int fd, int p);

queue_t *create_new_queue(int queue_size) {
    queue_t *q = (queue_t *)malloc(sizeof(queue_t));
    if (!q)
        return q;

    q->count = 0;
    q->queue_size = queue_size;
    q->head = NULL;
    q->tail = NULL;

    return q;
}

void delete_queue(void *self) {
    if (!self)
        return;

    node_t *curr = ((queue_t *)self)->head;
    while (curr) {
        node_t *tmp = curr->next;
        free(curr);
        curr = tmp;
    }

    free(self);
}

bool queue_is_empty(const queue_t *self) {
    assert(self);

    return self->count == 0 ? true : false;
}

bool enqueue(queue_t *self, int fd, int p) {
    assert(self);

    if (self->count == self->queue_size)
        return false;

    node_t *new_node = create_new_node(fd, p);
    if (!new_node)
        return false;

    if (!self->head) {
        self->head = new_node;
        self->tail = new_node;
    } else {
        self->tail->next = new_node;
        new_node->prev = self->tail;
        self->tail = new_node;
    }

    self->count++;
    return true;
}

int dequeue(queue_t *self) {
    assert(!queue_is_empty(self));

    int target_fd = self->head->fd;

    if (self->head == self->tail) {
        free(self->head);
        self->head = NULL;
        self->tail = NULL;
    } else {
        node_t *curr = self->head;
        self->head = self->head->next;
        self->head->prev = NULL;
        free(curr);
    }

    self->count--;
    return target_fd;
}

node_t *create_new_node(int fd, int p) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));

    if (!new_node)
        return new_node;

    new_node->fd = fd;
    new_node->priority = p;
    new_node->next = NULL;
    new_node->prev = NULL;

    return new_node;
}
