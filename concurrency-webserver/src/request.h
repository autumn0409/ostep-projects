#ifndef __REQUEST_H__

void request_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void request_handle(int fd);

#endif // __REQUEST_H__
