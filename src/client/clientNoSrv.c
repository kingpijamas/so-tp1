#include "../../include/client.h"
#include <stdio.h>
#include <stdlib.h>

// displays available data for product of name == prodname
clt_ret_code clt_show_product(product_name name){
	Product product;
	int ret; 
	if ((ret = db_get_product_by_name(name, &product, SHOW, NULL, NULL)) == OK) {
		printf("{ name=%s, quantity=%d }\n", product.name, product.quantity);		
	}
	return ret;
}

// adds a new product to the stock - this is used as an initialization method
clt_ret_code clt_add_product(product_name name, int quantity){
	printf("%s\n","I'm at add_product");
	Product aux, product = product_new(name, quantity);
	int ret;
	if (db_get_product_by_name(name, &aux, ADD, NULL, NULL) == OK) {
		ret=PRODUCT_ALREADY_INIT;
		printf("Return value %d\n",ret);
		//ERRROR IMPORTANTE: El primer add me entra acá -no debería para nada porque la base de datos está vacía-
		return PRODUCT_ALREADY_INIT;
	}
	ret=db_save_product(product, ADD);
	printf("Return value %d\n",ret);
	return ret;
}

// removes a product from the stock altogether - the opposite of clt_add_product
clt_ret_code clt_remove_product(product_name name){
	return db_delete_product(name, REMOVE);
}

// adds 'quantity' products with name == prodname
clt_ret_code clt_deposit_product(product_name name, int quantity){
	Product product = product_new(name, quantity);
	if (quantity < 0) {
		return NEGATIVE_QUANTITY;
	}
	return db_update_product(product, DEPOSIT);
}

// removes 'quantity' in products with name == prodname - the opposite of clt_deposit_product
clt_ret_code clt_take_product(product_name name, int quantity){
	Product product = product_new(name, -quantity);
	if (quantity < 0) {
		return NEGATIVE_QUANTITY;
	}
	return db_update_product(product, TAKE);
// db_ret_code db_update_product(Product product);
}