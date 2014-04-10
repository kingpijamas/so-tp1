#include "../include/productDB.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h> // for perror and remove

static void __write_new(Product product);
static FILE * __open(char * name, const char * mode);
static char * __get_path_to_tuple(char * name);

static boolean init = false;
static char buf[BUFFER_SIZE];


db_ret_code db_init() {
	if (init) { // maybe print that it was initialized several times, but it's entirely not critical
		return OK;
	}
	// read/write/search permissions for owner and group, and with read/search permissions for others
	if (mkdir(DB_ROOT_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_DATABASE;
	}
	if (mkdir(TABLE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_TABLE;
	}
	init = true;
	return OK;
}

db_ret_code db_save_product(Product product) {
	int getVal;
	if (!init) {
		return DB_NOT_INITIALIZED;
	}

	getVal = db_get_product_by_name(product.name, &product);
	switch (getVal) {
		case OK:
			return PRODUCT_EXISTS;
		case NO_PRODUCT_FOR_NAME:
			__write_new(product);
			return OK;
		default:
			return getVal;
	}
}

db_ret_code db_get_product_by_name(char * name, Product * productp) {
	Product rdProduct;
	rdProduct.name=malloc(MAX_NAME_DB);
	if (!init) {
		return DB_NOT_INITIALIZED;
	}

	FILE * file = __open(name, "r");
	if (file == NULL) {
		return NO_PRODUCT_FOR_NAME;
	}
	rdProduct.name = name;
	while(fscanf(file,"%d\n", &((rdProduct).quantity)) != EOF) {;}
	fclose(file);
	(*productp).name=rdProduct.name;
	(*productp).quantity=rdProduct.quantity;
	printf("Name=%s, quantity=%d\n", (*productp).name, (*productp).quantity);
	return OK;
}

db_ret_code db_update_product(Product product) {
	int getVal;
	if (!init) {
		return DB_NOT_INITIALIZED;
	}
	getVal = db_get_product_by_name(product.name, &product);
	switch (getVal) {
		case NO_PRODUCT_FOR_NAME:
			return NO_PRODUCT_FOR_NAME;
		case OK:
			__write_new(product);
			return OK;
		default:
			return UNEXPECTED_UPDATE_ERROR;
	}
}

db_ret_code db_delete_product(char * name) {
	Product product;
	int getVal; 
	if (!init) {
		return DB_NOT_INITIALIZED;
	}

	getVal = db_get_product_by_name(name, &product);
	switch (getVal) {
		case OK:
			if (remove(__get_path_to_tuple(name)) != 0){
				return UNEXPECTED_DELETE_ERROR;
			}
			return OK;
		default:
			return getVal; // this should probably be UNEXPECTED_DELETE_ERROR
	}
}

void __write_new(Product product) {
	FILE * file = __open(product.name, "w");
	fprintf(file, "%d\n", product.quantity);
	fclose(file);
}

FILE * __open(char * name, const char * mode) {
	return fopen(__get_path_to_tuple(name), mode);
}

char * __get_path_to_tuple(char * name) {
	sprintf(buf, "%s/%s", TABLE_PATH, name); //this should clear the buffer (verify!)
	return buf;
}