#ifndef _MODEL_
#define _MODEL_
#define PRODUCT_MAX_NAME_LEN 1000
#include "common.h"

typedef struct {
	string name;
	int quantity;
} Product;

void product_init(Product * product);

#endif
