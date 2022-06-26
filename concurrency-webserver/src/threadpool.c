#include "threadpool.h"

#include <stdlib.h>
#include <unistd.h>

#include "io_helper.h"
#include "request.h"

typedef enum {
    immediate_shutdown = 1,
    graceful_shutdown = 2
} threadpool_shutdown_t;

/**
 * @function void *threadpool_thread(void *threadpool)
 * @brief the worker thread
 * @param threadpool the pool which own the thread
 */
static void *threadpool_thread(void *threadpool);

int threadpool_free(threadpool_t *pool);

threadpool_t *threadpool_create(int thread_count, int queue_size) {
    threadpool_t *pool;

    if (thread_count <= 0 || queue_size <= 0) {
        return NULL;
    }

    if ((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL) {
        goto err;
    }

    /* Initialize */
    pool->thread_count = 0;
    pool->shutdown = pool->started = 0;

    /* Allocate thread and task queue */
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
    pool->task_queue = create_new_queue(queue_size);

    /* Initialize mutex and conditional variable first */
    if ((pthread_mutex_init(&(pool->lock), NULL) != 0) ||
        (pthread_cond_init(&(pool->notify), NULL) != 0) ||
        (pool->threads == NULL) ||
        (pool->task_queue == NULL)) {
        goto err;
    }

    /* Start worker threads */
    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&(pool->threads[i]), NULL,
                           threadpool_thread, (void *)pool) != 0) {
            threadpool_destroy(pool, 0);
            return NULL;
        }
        pool->thread_count++;
        pool->started++;
    }

    return pool;

err:
    if (pool) {
        threadpool_free(pool);
    }
    return NULL;
}

int threadpool_add(threadpool_t *pool, node_t *task_node) {
    int err = 0;

    if (pool == NULL) {
        return threadpool_invalid;
    }

    if (pthread_mutex_lock(&(pool->lock)) != 0) {
        return threadpool_lock_failure;
    }

    do {
        /* Are we shutting down ? */
        if (pool->shutdown) {
            err = threadpool_shutdown;
            break;
        }

        /* Add task to queue */
        if (enqueue(pool->task_queue, task_node) != 0) {
            err = threadpool_queue_full;
            break;
        }

        /* pthread_cond_broadcast */
        if (pthread_cond_signal(&(pool->notify)) != 0) {
            err = threadpool_lock_failure;
            break;
        }
    } while (0);

    if (pthread_mutex_unlock(&pool->lock) != 0) {
        err = threadpool_lock_failure;
    }

    return err;
}

int threadpool_destroy(threadpool_t *pool, int flags) {
    int err = 0;

    if (pool == NULL) {
        return threadpool_invalid;
    }

    if (pthread_mutex_lock(&(pool->lock)) != 0) {
        return threadpool_lock_failure;
    }

    do {
        /* Already shutting down */
        if (pool->shutdown) {
            err = threadpool_shutdown;
            break;
        }

        pool->shutdown = (flags & threadpool_graceful) ? graceful_shutdown : immediate_shutdown;

        /* Wake up all worker threads */
        if ((pthread_cond_broadcast(&(pool->notify)) != 0) ||
            (pthread_mutex_unlock(&(pool->lock)) != 0)) {
            err = threadpool_lock_failure;
            break;
        }

        /* Join all worker thread */
        for (int i = 0; i < pool->thread_count; i++) {
            if (pthread_join(pool->threads[i], NULL) != 0) {
                err = threadpool_thread_failure;
            }
        }
    } while (0);

    /* Only if everything went well do we deallocate the pool */
    if (!err) {
        threadpool_free(pool);
    }
    return err;
}

int threadpool_free(threadpool_t *pool) {
    if (pool == NULL || pool->started > 0) {
        return -1;
    }

    /* Free task queue */
    delete_queue(pool->task_queue);
    free(pool->task_queue);

    /* Did we manage to allocate ? */
    if (pool->threads) {
        free(pool->threads);

        /* Because we allocate pool->threads after initializing the
           mutex and condition variable, we're sure they're
           initialized. Let's lock the mutex just in case. */
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->notify));
    }
    free(pool);
    return 0;
}

static void *threadpool_thread(void *threadpool) {
    threadpool_t *pool = (threadpool_t *)threadpool;

    for (;;) {
        /* Lock must be taken to wait on conditional variable */
        pthread_mutex_lock(&(pool->lock));

        /* Wait on condition variable, check for spurious wakeups.
           When returning from pthread_cond_wait(), we own the lock. */
        while (pool->task_queue->count == 0 && (!pool->shutdown)) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if ((pool->shutdown == immediate_shutdown) ||
            ((pool->shutdown == graceful_shutdown) &&
             pool->task_queue->count == 0)) {
            break;
        }

        /* Grab our task */
        node_t *task_node = dequeue(pool->task_queue);

        /* Unlock */
        pthread_mutex_unlock(&(pool->lock));

        /* Get to work */
        request_handle(task_node);

        /* close connection */
        close_or_die(task_node->fd);

        /* free node */
        free(task_node);
    }

    pool->started--;

    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
    return (NULL);
}