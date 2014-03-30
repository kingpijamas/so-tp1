#ifndef PRODUCT_DB_H
#define PRODUCT_DB_H

#include "common.h"
#include "model.h"
#include "utils.h"

#define DB_ROOT_PATH "db"
#define TABLE_NAME "product"

typedef enum {
	CANNOT_CREATE_DATABASE = OK+1,
	CANNOT_CREATE_TABLE,
	NO_PRODUCT_FOR_NAME,
	PRODUCT_EXISTS,
	UNEXPECTED_DELETE_ERROR,
	UNEXPECTED_UPDATE_ERROR
} db_ret_code;

db_ret_code save_product(Product product);
db_ret_code get_product_by_name(char * name, Product * productp);
db_ret_code update_product(Product product);
db_ret_code delete_product(char * name);

#endif