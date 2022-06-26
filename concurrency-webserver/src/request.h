#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "node.h"

void request_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
int request_preprocessing(node_t *new_node);
void request_handle(node_t *task_node);

#endif  // __REQUEST_H__
