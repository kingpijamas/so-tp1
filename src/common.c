#include "../include/common.h"

void
fatal(char *s)
{
	perror(s);
	exit(1);
}