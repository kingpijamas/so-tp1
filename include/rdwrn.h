#ifndef _RDWRN_
#define _RDWRN_

#include <sys/types.h>

ssize_t readn(int fd, void *buf, size_t n);
ssize_t writen(int fd, const void *buf, size_t len);

#endif