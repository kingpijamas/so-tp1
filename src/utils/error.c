#include "../../include/error.h"

void assert(boolean val, string error_text) {
	if (!val) {
		fail(error_text);
	}
}

void fail(string error_text) {
	int errnost = errno;
	perror(error_text);
	printf("%s (errno=%d)\n", error_text, errnost);
	exit(1);
}