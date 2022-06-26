#include "queue.h"

#include <assert.h>
#include <stdlib.h>

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

int enqueue(queue_t *self, int fd, int p) {
    assert(self);

    if (self->count == self->queue_size)
        return -1;

    node_t *new_node = create_new_node(fd, p);
    if (!new_node)
        return -2;

    if (!self->head) {
        self->head = new_node;
        self->tail = new_node;
    } else {
        // find insert position
        node_t *left_node = self->tail;
        while (left_node && left_node->priority > p)
            left_node = left_node->prev;

        // insert before head
        if (!left_node) {
            self->head->prev = new_node;
            new_node->next = self->head;
            self->head = new_node;
        }
        // insert after tail
        else if (left_node == self->tail) {
            self->tail->next = new_node;
            new_node->prev = self->tail;
            self->tail = new_node;
        }
        // insert bewtween 2 exist nodes
        else {
            node_t *right_node = left_node->next;
            left_node->next = new_node;
            new_node->prev = left_node;
            right_node->prev = new_node;
            new_node->next = right_node;
        }        
    }

    self->count++;
    return 0;
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
