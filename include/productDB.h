#ifndef _PRODUCT_DB_
#define _PRODUCT_DB_

#include "common.h"
#include "model.h"

#define DB_ROOT_PATH "db"
#define TABLE_NAME "product"

typedef enum {
	DB_NOT_INITIALIZED = OK+1,
	CANNOT_CREATE_DATABASE,
	CANNOT_CREATE_TABLE,
	NO_PRODUCT_FOR_NAME,
	PRODUCT_EXISTS,
	UNEXPECTED_DELETE_ERROR,
	UNEXPECTED_UPDATE_ERROR
} db_ret_code;

db_ret_code db_init();
db_ret_code db_save_product(Product product);
db_ret_code db_get_product_by_name(char * name, Product * productp);
db_ret_code db_update_product(Product product);
db_ret_code db_delete_product(char * name);

#endif