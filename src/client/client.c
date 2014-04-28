#include <stdio.h>
#include <stdlib.h>
#include "../../include/client.h"

// displays available data for product of name == prodname
db_ret_code show_product(product_name name){
	Product product;
	int ret;
	ret=db_get_product_by_name(name, &product);
	printf("Name=%s, quantity=%d\n", product.name, product.quantity);
	return ret;
}

// adds a new product to the stock - this is used as an initialization method
db_ret_code add_product(product_name name, int quantity){
	Product product = product_new(name, quantity);
	return db_save_product(product);
}

// removes a product from the stock altogether - the opposite of add_product
db_ret_code remove_product(product_name name){
	return db_delete_product(name);
}

// adds 'quantity' products with name == prodname
db_ret_code deposit_product(product_name name, int quantity){
	Product product = product_new(name, quantity);
	return db_update_product(product);
}

// removes 'quantity' in products with name == prodname - the opposite of deposit_product
db_ret_code take_product(product_name name, int quantity){
	Product product = product_new(name, -quantity);
	return db_update_product(product);
// db_ret_code db_update_product(Product product);
}