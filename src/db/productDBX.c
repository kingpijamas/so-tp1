#include "../../include/productDB.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h> // for perror and remove
#include "../../include/error.h"

#define DB_TABLE_LOCK_FILE ".db-lock"
#define DB_FULL_PATH_TO_LOCK_FILE DB_TABLE_PATH"/"DB_TABLE_LOCK_FILE

static void __write_new_product(Product product);
static FILE * __open(string name, const string mode);
static string __get_path_to_tuple(string name);
static FILE * __lock_table(short l_type);
static void __unlock_table(FILE * table_lock_file);
static int __lock_file(FILE * file, short l_type);
static db_ret_code __save_product(Product product);
static db_ret_code __get_product_by_name(product_name name, Product * productp);
static db_ret_code __update_product(Product product);
static db_ret_code __delete_product(product_name name);

#define __verify_init()	verify(__init, "DB: Not initialized")
#define __unlock_file(file) __lock_file(file, F_UNLCK)


static boolean __init = false;
static char __buf[DB_BUFFER_SIZE];

db_ret_code db_init() {
	if (__init) { // maybe print that it was __initialized several times, but it's entirely not critical
		return OK;
	}
	// read/write/search permissions for owner and group, and with read/search permissions for others
	if (mkdir(DB_ROOT_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_DATABASE;
	}
	if (mkdir(DB_TABLE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_TABLE;
	}
	fclose(fopen(DB_FULL_PATH_TO_LOCK_FILE, "w"));
	__init = true;
	return OK;
}

db_ret_code db_get_product_by_name(product_name name, Product * productp) {
	FILE * table_lock_file;
	int ret;

	__verify_init();

	table_lock_file = __lock_table(F_RDLCK);
	ret = __get_product_by_name(name, productp);
	__unlock_table(table_lock_file);
	
	return ret;
}

db_ret_code db_save_product(Product product) {
	FILE * table_lock_file;
	int ret;

	__verify_init();

	table_lock_file = __lock_table(F_WRLCK);
	ret = __save_product(product);
	__unlock_table(table_lock_file);
	
	return ret;
}

db_ret_code db_update_product(Product product) {
	FILE * table_lock_file;
	int ret;

	__verify_init();

	table_lock_file = __lock_table(F_WRLCK);
	ret = __update_product(product);
	__unlock_table(table_lock_file);

	return ret;
}

db_ret_code db_delete_product(product_name name) {
	FILE * table_lock_file;
	int ret;

	__verify_init();

	table_lock_file = __lock_table(F_WRLCK);
	ret = __delete_product(name);
	__unlock_table(table_lock_file);
	return ret;
}

db_ret_code __save_product(Product product) {
	int getVal = __get_product_by_name(product.name, &product);
	switch (getVal) {
		case OK:
			return PRODUCT_EXISTS;
		case NO_PRODUCT_FOR_NAME:
			__write_new_product(product);
			return OK;
		default:
			return getVal;
	}
}

db_ret_code __get_product_by_name(product_name name, Product * productp) {
	Product rdProduct;

	FILE * file = __open(name, "r");
	if (file == NULL) {
		return NO_PRODUCT_FOR_NAME;
	}
	product_set_name(&rdProduct, name);
	while(fscanf(file,"%d\n", &((rdProduct).quantity)) != EOF) {;}
	fclose(file);
	*productp = rdProduct;
	return OK;
}

db_ret_code __update_product(Product product) {
	int getVal;
	Product originalProduct;

	getVal = __get_product_by_name(product.name, &originalProduct);
	switch (getVal) {
		case NO_PRODUCT_FOR_NAME:
			return NO_PRODUCT_FOR_NAME;
		case OK:
			product_set_quantity(&product, product.quantity+originalProduct.quantity);
			__write_new_product(product);
			return OK;
		default:
			return UNEXPECTED_UPDATE_ERROR;
	}
}

db_ret_code __delete_product(product_name name) {
	Product product;
	int getVal; 

	getVal = __get_product_by_name(name, &product);
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

void __write_new_product(Product product) {
	FILE * file = __open(product.name, "w");
	fprintf(file, "%d\n", product.quantity);
	fclose(file);
}

FILE * __open(product_name name, const string mode) {
	return fopen(__get_path_to_tuple(name), mode);
}

string __get_path_to_tuple(product_name name) {
	sprintf(__buf, "%s/%s", DB_TABLE_PATH, name); //this should clear the buffer (verify!)
	return __buf;
}

FILE * __lock_table(short l_type) {
	FILE * table_lock_file = fopen(DB_FULL_PATH_TO_LOCK_FILE, (l_type == F_WRLCK)? "w":"r");
	__lock_file(table_lock_file, l_type);
	return table_lock_file;
}

void __unlock_table(FILE * table_lock_file) {
	__unlock_file(table_lock_file);
	fclose(table_lock_file);
}

int __lock_file(FILE * file, short l_type) {
	int fd;
	struct flock args;
	args.l_type = l_type;
	args.l_whence = SEEK_SET; // from the beginning of the file
	args.l_start = 0; // offset
	args.l_len = 0; // the whole file, no matter how much it may grow
	args.l_pid = 0; // the pid is unset (it's a F_SETLKW)

	verify((fd = fileno(file)) != -1, "DB: Lock error");
	verify(fcntl(fd, F_SETLKW, &args) != -1, "DB: Lock error");
	return OK;
}