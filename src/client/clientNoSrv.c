#include "../../include/client.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../include/productDBX.h"

// displays available data for product of name == prodname
void show_product(product_name name){
	Product product;
	dbx_get_product_by_name(name, &product);
	printf("Name=%s, quantity=%d\n", product.name, product.quantity);
}

// adds a new product to the stock - this is used as an initialization method
void add_product(product_name name, int quantity){
	Product product = product_new(name, quantity);
	dbx_save_product(product);
}

// removes a product from the stock altogether - the opposite of add_product
void remove_product(product_name name){
	dbx_delete_product(name);
}

// adds 'quantity' products with name == prodname
void deposit_product(product_name name, int quantity){
	Product product = product_new(name, quantity);
	dbx_update_product(product);
}

// removes 'quantity' in products with name == prodname - the opposite of deposit_product
void take_product(product_name name, int quantity){
	Product product = product_new(name, -quantity);
	dbx_update_product(product);
// db_ret_code db_update_product(Product product);
}