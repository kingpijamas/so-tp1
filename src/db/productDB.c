#include "../../include/productDB.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h> // for perror and remove

static void __write_new(Product product);
static FILE * __open(string name, const string mode);
static string __get_path_to_tuple(string name);

static boolean init = false;
static char buf[DB_BUFFER_SIZE];


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

db_ret_code db_get_product_by_name(product_name name, Product * productp) {
	Product rdProduct;

	if (!init) {
		return DB_NOT_INITIALIZED;
	}
//	printf("Hola moni\n");
	FILE * file = __open(name, "r");
//	printf("Hola pepe\n");
	if (file == NULL) {
		return NO_PRODUCT_FOR_NAME;
	}
	product_set_name(&rdProduct, name);
	while(fscanf(file,"%d\n", &((rdProduct).quantity)) != EOF) {;}
	fclose(file);
	*productp = rdProduct;
	return OK;
}

db_ret_code db_update_product(Product product) {
	int getVal;
	Product originalProduct;

//printf("A\n");
	if (!init) {
		return DB_NOT_INITIALIZED;
	}
//printf("B\n");
	getVal = db_get_product_by_name(product.name, &originalProduct);
//printf("C\n");
	switch (getVal) {
		case NO_PRODUCT_FOR_NAME:
			return NO_PRODUCT_FOR_NAME;
		case OK:
			product_set_quantity(&product, product.quantity+originalProduct.quantity);
			__write_new(product);
			return OK;
		default:
			return UNEXPECTED_UPDATE_ERROR;
	}
}

db_ret_code db_delete_product(product_name name) {
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

FILE * __open(product_name name, const string mode) {
	//printf("<__open\n");
	FILE * ans = fopen(__get_path_to_tuple(name), mode);
	//printf("__open/>\n");
	return ans;
}

string __get_path_to_tuple(product_name name) {
	//string param;
	//printf("<__get_path_to_tuple\n");
	//param = malloc(strlen(TABLE_PATH)+strlen(name)+2);
	sprintf(buf, "%s/%s", TABLE_PATH, name); //this should clear the buffer (verify!)
	//sprintf(param, "%s/%s", TABLE_PATH, name);
	//printf("%s __get_path_to_tuple/>\n", param);
	//printf("%s __get_path_to_tuple/>\n", buf);
	return buf;
	//return param;
}