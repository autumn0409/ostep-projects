#include <stdio.h>
#include <pthread.h>

#include "io_helper.h"
#include "request.h"

char default_root[] = ".";

//
// ./wserver [-d <basedir>] [-p <portnum>]
//
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
    pthread_t tid[2];

    while ((c = getopt(argc, argv, "d:p:")) != -1)
        switch (c) {
            case 'd':
                root_dir = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
                exit(1);
        }

    // run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    int t_cnt = 0;
    while (1) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        int conn_fd = accept_or_die(listen_fd, (sockaddr_t *)&client_addr, (socklen_t *)&client_len);
        if (pthread_create(&tid[t_cnt++], NULL, request_handle, &conn_fd) != 0) {
            fprintf(stderr, "failed to create thread\n");
            exit(1);
        }
        if (t_cnt >= 2) {
            int i = 0;
            pthread_join(tid[i++], NULL);
            t_cnt = 0;
        }
    }
    return 0;
}
