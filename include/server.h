#ifndef _SERVER_
#define _SERVER_

#include "common.h"
#include "product.h"
#include "communicator.h"
#include "productDB.h"
#include "msg.h"

#define SRV_ID 0

typedef enum {
	INVALID_MSG = OK+1
} srv_ret_code;

void srv_start();
srv_ret_code srv_get_product(product_name name, Product * productp);
srv_ret_code srv_write_product(Product * productp);
srv_ret_code srv_remove_product(product_name name);

#endif
