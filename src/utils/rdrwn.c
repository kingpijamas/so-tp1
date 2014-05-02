#include <errno.h>
#include <unistd.h>

ssize_t freadn(int fd, void *vptr, size_t n) {
	size_t nleft;
	ssize_t nread;
	char *ptr;

	for (ptr = vptr, nleft = n; nleft > 0; nleft -= nread, ptr += nread){
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR) {
				nread = 0;
			} else {
				return -1;
			}
		} else if (nread == 0) {
			break;
		}
	}
	return n - nleft;		/* 0 <= result <= n */
}

ssize_t fwriten(int fd, const void *vptr, size_t n) {
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	for (ptr = vptr, nleft = n; nleft > 0; nleft -= nwritten, ptr += nwritten){
		if ((nwritten = write(fd, ptr, nleft)) < 0) {
			if (errno == EINTR) {
				nwritten = 0;
			} else {
				return -1;
			}
		} else if (nwritten == 0) {
			break;
		}
	}
	return n - nleft;		/* 0 <= result <= n */
}