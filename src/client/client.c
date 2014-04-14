#include <stdio.h>
#include <stdlib.h>
#include "../../include/client.h"
#include "../../include/productDBX.h"

// displays available data for product of name == prodname
void show_product(string prodname){
	Product product;
	product_init(&product);

	dbx_get_product_by_name(prodname, &product);
	printf("Name=%s, quantity=%d\n", product.name, product.quantity);
}

// adds a new product to the stock - this is used as an initialization method
void add_product(string prodname, int quantity){
	Product product;
	product_init(&product);
	product.name=prodname;
	product.quantity=quantity;

	dbx_save_product(product);
}

// removes a product from the stock altogether - the opposite of add_product
void remove_product(string prodname){
	dbx_delete_product(prodname);

}

// adds 'quantity' products with name == prodname
void deposit_product(string prodname, int quantity){
	Product product;
	product_init(&product);
	product.name=prodname;
	product.quantity=quantity;

	dbx_update_product(product);
}

// removes 'quantity' in products with name == prodname - the opposite of deposit_product
void take_product(string prodname, int quantity){
	Product product;
	product_init(&product);
	product.name=prodname;
	product.quantity=-quantity;

	dbx_update_product(product);
// db_ret_code db_update_product(Product product);
}