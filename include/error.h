#ifndef _ERROR_
#define _ERROR_

#include "common.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void fail(string error_text);
void assert(boolean val, string error_text);

#endif