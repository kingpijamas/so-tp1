#ifndef _COMMON_
#define _COMMON_

#include "model.h"

#define OK 0
#define ERROR -1
#define MAX_NAME 1000

typedef enum { false,true } boolean;

typedef char * string;

void product_init(Product * product);

#endif
