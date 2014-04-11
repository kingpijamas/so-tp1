#ifndef _CLIENT_
#define _CLIENT_

#include "common.h"
#include "model.h"
#include "communicator.h"

// displays available data for product of name == prodname
void show_product(char * prodname);

// adds a new product to the stock - this is used as an initialization method
void add_product(char * prodname, int quantity);
// removes a product from the stock altogether - the opposite of add_product
void remove_product(char * prodname);

// Adds 'quantity' to product with name == prodname
void deposit_product(char * prodname, int quantity);
// removes 'quantity' products with name == prodname - the opposite of deposit_product
void take_product(char * prodname, int quantity);

#endif
