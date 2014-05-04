#ifndef _RDWRN_
#define _RDWRN_

#include <sys/types.h>

ssize_t freadn(int fd, void *vptr, size_t n);
ssize_t fwriten(int fd, const void *vptr, size_t n);

#endif