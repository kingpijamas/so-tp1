#include "../../include/product.h"
#include <stdlib.h>
#include <string.h>
#include "../../include/utils.h"

Product product_new(product_name name, int quantity) {
	Product product;
	product_set_name(&product, name);
	product.quantity = quantity;
	return product;
}

//FIXME: use void *memcpy(void *dest, const void *src, size_t n);!
//FIXME: big names are just being chunked. A warning should at least be given (maybe an error?)
void product_set_name(Product * product, product_name name) {
	memcpy(product->name, name, min(sizeof(product_name),strlen(name)+1));

}

void product_set_quantity(Product * product, int quantity) {
	product -> quantity = quantity;
}