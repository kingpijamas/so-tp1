#ifndef _PRODUCT_DBX_
#define _PRODUCT_DBX_

#include "productDB.h"

//locks.h
typedef enum {
	READ_MODE, 
	WRITE_MODE
} lock_mode;

 db_ret_code dbx_save_product(Product product);
 db_ret_code dbx_get_product_by_name(string name, Product * productp);
 db_ret_code dbx_update_product(Product product);
 db_ret_code dbx_delete_product(string name);

 #endif