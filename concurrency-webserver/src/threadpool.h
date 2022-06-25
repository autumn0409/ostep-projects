#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct threadpool_t threadpool_t;

typedef enum {
    threadpool_queue_full = -1,
    threadpool_queue_malloc_failure = -2
    threadpool_invalid = -3,
    threadpool_lock_failure = -4,
    threadpool_shutdown = -5,
    threadpool_thread_failure = -6
} threadpool_error_t;

typedef enum {
    threadpool_graceful = 1
} threadpool_destroy_flags_t;

/**
 * @function threadpool_create
 * @brief Creates a threadpool_t object.
 * @param thread_count Number of worker threads.
 * @param queue_size   Size of the queue.
 * @return a newly created thread pool or NULL
 */
threadpool_t *threadpool_create(int thread_count, int queue_size);

/**
 * @function threadpool_add
 * @brief add a new task in the queue of a thread pool
 * @param pool     Thread pool to which add the task.
 * @param fd Socket file desciptor to be passed to the request function.
 * @return 0 if all goes well, negative values in case of error (@see
 * threadpool_error_t for codes).
 */
int threadpool_add(threadpool_t *pool, int fd);

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