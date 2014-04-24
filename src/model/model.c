#include "../../include/model.h"
#include <stdlib.h>

Product product_new(product_name name, int quantity) {
	Product product;
	product_set_name(&product, name);
	product.quantity = quantity;
	return product;
}

void product_set_name(Product * product, product_name name) {
	int i;
	for(i=0; i<sizeof(name); i++) {
		product->name[i]=name[i];
	}
}