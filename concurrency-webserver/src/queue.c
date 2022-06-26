#include "queue.h"

#include <assert.h>
#include <stdlib.h>

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

void delete_queue(queue_t *self) {
    if (!self)
        return;

    node_t *curr = self->head;
    while (curr) {
        node_t *tmp = curr->next;
        free(curr);
        curr = tmp;
    }

    free(self);
}

int enqueue(queue_t *self, node_t *new_node) {
    assert(self);

    if (self->count == self->queue_size)
        return -1;

    if (!self->head) {
        self->head = new_node;
        self->tail = new_node;
    } else {
        // find insert position
        node_t *left_node = self->tail;
        while (left_node && left_node->file_st_size > new_node->file_st_size)
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

node_t *dequeue(queue_t *self) {
    assert(self->count > 0);

    node_t *target_node = self->head;

    if (self->head == self->tail) {
        self->head = NULL;
        self->tail = NULL;
    } else {
        self->head = self->head->next;
        self->head->prev = NULL;
    }

    self->count--;
    target_node->next = NULL;
    return target_node;
}