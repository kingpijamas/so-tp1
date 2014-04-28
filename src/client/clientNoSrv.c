#include "../../include/client.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../include/productDBX.h"

// displays available data for product of name == prodname
clt_ret_code clt_show_product(product_name name){
	Product product;
	int ret; 
	if ((ret = dbx_get_product_by_name(name, &product)) == OK) {
		printf("{ name=%s, quantity=%d }\n", product.name, product.quantity);		
	}
	return ret;
}

// adds a new product to the stock - this is used as an initialization method
clt_ret_code clt_add_product(product_name name, int quantity){
	Product aux, product = product_new(name, quantity);
	if (dbx_get_product_by_name(name, &aux) == OK) {
		return PRODUCT_ALREADY_INIT;
	}
	return dbx_save_product(product);
}

// removes a product from the stock altogether - the opposite of clt_add_product
clt_ret_code clt_remove_product(product_name name){
	return dbx_delete_product(name);
}

// adds 'quantity' products with name == prodname
clt_ret_code clt_deposit_product(product_name name, int quantity){
	Product product = product_new(name, quantity);
	if (quantity < 0) {
		return NEGATIVE_QUANTITY;
	}
	return dbx_update_product(product);
}

// removes 'quantity' in products with name == prodname - the opposite of clt_deposit_product
clt_ret_code clt_take_product(product_name name, int quantity){
	Product product = product_new(name, -quantity);
	if (quantity < 0) {
		return NEGATIVE_QUANTITY;
	}
	return dbx_update_product(product);
// db_ret_code db_update_product(Product product);
}