#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "io_helper.h"
#include "request.h"
#include "threadpool.h"

char default_root[] = ".";
char default_schedalg[] = "FIFO";

//
// ./wserver [-d <basedir>] [-p <portnum>] [-t threads] [-b buffers] [-s schedalg]
//
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    char *schedalg = default_schedalg;
    int port = 10000, thread_count = 1, queue_size = 1;

    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1)
        switch (c) {
            case 'd':
                root_dir = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 't':
                thread_count = atoi(optarg);
                break;
            case 'b':
                queue_size = atoi(optarg);
                break;
            case 's':
                schedalg = optarg;
                if (strcmp(schedalg, "FIFO") != 0 && strcmp(schedalg, "SFF") != 0) {
                    fprintf(stderr, "scheduling algorithm must be one of FIFO or SFF\n");
                    exit(1);
                }
                break;
            default:
                fprintf(stderr,
                        "usage: wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]\n");
                exit(1);
        }

    // run out of this directory
    chdir_or_die(root_dir);

    // create thread pool
    threadpool_t *pool;
    assert((pool = threadpool_create(thread_count, queue_size, schedalg)) != NULL);
    printf("Thread pool started with %d threads and queue size of %d.\n",
           thread_count, queue_size);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    printf("Server start listening...\n");
    while (1) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        int conn_fd = accept_or_die(listen_fd, (sockaddr_t *)&client_addr,
                                    (socklen_t *)&client_len);

        node_t *new_node = create_new_node(conn_fd);
        if (!new_node) {
            request_error(conn_fd, NULL, "500", "Add task failed",
                          "malloc failed while adding task to buffer");
            close_or_die(conn_fd);
            continue;
        }

        if (request_preprocessing(new_node) != 0) {
            request_error(conn_fd, new_node->filename,
                          "404", "Not found", "server could not find this file");
            free(new_node);
            close_or_die(conn_fd);
            continue;
        }

        int err = threadpool_add(pool, new_node);
        if (err == 0)
            continue;
        else if (err == -1) {
            request_error(conn_fd, NULL, "500", "Task buffer full",
                          "task buffer is full now, try again latter");
            close_or_die(conn_fd);
            free(new_node);
            continue;
        } else {
            fprintf(stderr, "thread pool error: %d.\n", err);
            close_or_die(conn_fd);
            free(new_node);
            exit(err);
        }
    }
    return 0;
}
