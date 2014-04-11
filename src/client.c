#include <stdio.h>
#include <stdlib.h>
#include "../include/client.h"
#include "../include/dbx.h"

// displays available data for product of name == prodname
void show_product(char * prodname){
	Product product;
	product.name=malloc(MAX_NAME_CLIENT);
	dbx_get_product_by_name(prodname, &product);
	printf("Name=%s, quantity=%d\n", product.name, product.quantity);
}

// adds a new product to the stock - this is used as an initialization method
void add_product(char * prodname, int quantity){

}

// removes a product from the stock altogether - the opposite of add_product
void remove_product(char * prodname){

}

// adds 'quantity' products with name == prodname
void deposit_product(char * prodname, int quantity){

}

// removes 'quantity' products with name == prodname - the opposite of deposit_product
void take_product(char * prodname, int quantity){

}