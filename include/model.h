#ifndef _MODEL_
#define _MODEL_

#include "common.h"

#define PRODUCT_MAX_NAME_LEN 1000

typedef struct {
	string name;
	int quantity;
} Product;

void product_init(Product * product);

#endif
