#include "../../include/client.h"
#include <stdio.h>
#include <stdlib.h>

// displays available data for product of name == prodname
clt_ret_code clt_show_product(product_name name){
	db_init();

	Product product;
	int ret;
	
	printf("Showing product... ");
	if ((ret = db_get_product_by_name(name, &product)) == OK) {
		printf("\n\n{ name=%s, quantity=%d }\n\n", product.name, product.quantity);		
	}
	return ret;
}

// adds a new product to the stock - this is used as an initialization method
clt_ret_code clt_add_product(product_name name, int quantity){
	db_init();

	Product aux, product = product_new(name, quantity);
	int ret;

	printf("Adding product... { name=%s, quantity=%d }\n", name, quantity);
	if (db_get_product_by_name(name, &aux) == OK) {
		ret = PRODUCT_ALREADY_INIT;
		//printf("Return value %d\n",ret);
		return PRODUCT_ALREADY_INIT;
	}
	ret = db_save_product(product);
	//printf("Return value %d\n",ret);
	return ret;
}

// removes a product from the stock altogether - the opposite of clt_add_product
clt_ret_code clt_remove_product(product_name name){
	db_init();

	printf("Removing product... { name=%s }\n", name);	
	return db_delete_product(name);
}

// adds 'quantity' products with name == prodname
clt_ret_code clt_deposit_product(product_name name, int quantity){
	db_init();
	
	Product product = product_new(name, quantity);
	printf("Depositing product... { name=%s, quantity=%d }\n", name, quantity);

	if (quantity < 0) {
		return NEGATIVE_QUANTITY;
	}
	return db_update_product(product);
}

// removes 'quantity' in products with name == prodname - the opposite of clt_deposit_product
clt_ret_code clt_take_product(product_name name, int quantity){
	db_init();
	
	Product product = product_new(name, -quantity);

	printf("Taking product... { name=%s, quantity=%d }\n", name, quantity);

	if (quantity < 0) {
		return NEGATIVE_QUANTITY;
	}
	return db_update_product(product);
}