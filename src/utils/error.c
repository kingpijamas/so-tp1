#include "../../include/error.h"

void assert(boolean val, string error_text) {
	if (!val) {
		fail(error_text);
	}
}

void fail(string error_text) {
	int errnost = errno;
	perror(error_text);
	printf("(errno=%d)\n", errnost);
	exit(1);
}