#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <pthread.h>

#include "node.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct threadpool_t threadpool_t;

typedef enum {
    threadpool_queue_full = -1,
    threadpool_invalid = -2,
    threadpool_lock_failure = -3,
    threadpool_shutdown = -4,
    threadpool_thread_failure = -5
} threadpool_error_t;

typedef enum {
    threadpool_graceful = 1
} threadpool_destroy_flags_t;

/**
 *  @struct threadpool
 *  @brief The threadpool struct
 *
 *  @var notify       Condition variable to notify worker threads.
 *  @var threads      Array containing worker threads ID.
 *  @var thread_count Number of threads
 *  @var task_queue   Priority queue containing the tasks.
 *  @var shutdown     Flag indicating if the pool is shutting down
 *  @var started      Number of started threads
 */
struct threadpool_t {
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    int thread_count;
    queue_t *task_queue;
    int shutdown;
    int started;
};

/**
 * @function threadpool_create
 * @brief Creates a threadpool_t object.
 * @param thread_count Number of worker threads.
 * @param queue_size   Size of the queue.
 * @param schedalg     Scheduling algorithm to be performed.
 * @return a newly created thread pool or NULL
 */
threadpool_t *threadpool_create(int thread_count, int queue_size, char *schedalg);

/**
 * @function threadpool_add
 * @brief add a new task in the queue of a thread pool
 * @param pool      Thread pool to which add the task.
 * @param task_node Task node to be queued into buffer.
 * @return 0 if all goes well, negative values in case of error (@see
 * threadpool_error_t for codes).
 */
int threadpool_add(threadpool_t *pool, node_t *task_node);

/**
 * @function threadpool_destroy
 * @brief Stops and destroys a thread pool.
 * @param pool  Thread pool to destroy.
 * @param flags Flags for shutdown
 *
 * Known values for flags are 0 (default) and threadpool_graceful in
 * which case the thread pool doesn't accept any new tasks but
 * processes all pending tasks before shutdown.
 */
int threadpool_destroy(threadpool_t *pool, int flags);

#ifdef __cplusplus
}
#endif

#endif /* _THREADPOOL_H_ */