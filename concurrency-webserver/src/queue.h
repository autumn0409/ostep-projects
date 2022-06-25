#ifndef QUEUE_H
#define QUEUE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef struct queue_t queue_t;

#ifdef __cplusplus
extern "C" {
#endif

queue_t *create_new_queue(int queue_size);
void delete_queue(void *self);
bool queue_is_empty(const queue_t *self);
bool enqueue(queue_t *self, int fd, int p);
int dequeue(queue_t *self);

#ifdef __cplusplus
}
#endif

#endif /* QUEUE_H */