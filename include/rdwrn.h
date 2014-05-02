#ifndef _RDWRN_
#define _RDWRN_

#include <sys/types.h>

//ssize_t readn(void *buf, size_t n);
//ssize_t writen(const void *buf, size_t n);
ssize_t freadn(int fd, void *buf, size_t n);
ssize_t fwriten(int fd, const void *buf, size_t len);


#endif