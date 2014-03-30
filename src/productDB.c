#include "../include/productDB.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h> // for perror and remove

#define TABLE_PATH DB_ROOT_PATH"/"TABLE_NAME

static int __init();
static void __write_new(Product product);
static FILE * __open(char * name, const char * mode);
static char * __get_path_to_tuple(char * name);

static char[NAME_LEN] buf;

db_ret_code save_product(Product product) {
	int getVal = get_product_by_name(product.name, &product);
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

db_ret_code get_product_by_name(char * name, Product * productp) {
	int initVal;
	Product rdProduct;

	FILE * file = __open(name, "r");
	if (file == NULL) {
		initVal = __init();
		return (initVal != OK)? initVal : NO_PRODUCT_FOR_NAME;
	}
	rdProduct.name = name;
	while(fscanf(file,"%d\n", &(rdProduct.quantity)) != EOF) {;}
	fclose(file);
	productp = &rdProduct;
	return OK;
}

db_ret_code update_product(Product product) {
	switch (get_product_by_name(product.name, &product)) {
		case NO_PRODUCT_FOR_NAME:
			return NO_PRODUCT_FOR_NAME;
		case OK:
			__write_new(product);
			return OK;
		default:
			return UNEXPECTED_UPDATE_ERROR;
	}
}

db_ret_code delete_product(char * name) {
	Product product;
	int getVal = get_product_by_name(name, &product);
	switch (getVal) {
		case OK:
			return PRODUCT_EXISTS;
		case NO_PRODUCT_FOR_NAME:
			if (remove(__get_path_to_tuple(name)) != 0){
				return UNEXPECTED_DELETE_ERROR;
			}
			return OK;
		default:
			return getVal;
	}
}

db_ret_code __init() {
	// read/write/search permissions for owner and group, and with read/search permissions for others
	if (mkdir(DB_ROOT_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_DATABASE;
	}
	if (mkdir(TABLE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_TABLE;
	}
	return OK;
}

void __write_new(Product product) {
	FILE * file = __open(product.name, "w");
	fprintf(file, "%d\n", product.quantity);
	fclose(file);
}

// Revisar sprintf con los cambios %s/%s
FILE * __open(char * name, const char * mode) {
	return fopen(__get_path_to_tuple(name), mode);
}

char * __get_path_to_tuple(char * name) {
	sprintf(buf, "%s/%s", TABLE_PATH, name); //this should clear the buffer (verify!)
	return buf;
}