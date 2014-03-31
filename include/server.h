#ifndef _SERVER_
#define _SERVER_

#include "common.h"
#include "model.h"
#include "communicator.h"

typedef enum { 
	INVALID_MSG = OK +1,
	RESOURCE_BUSY,
	ILLEGAL_PRODUCT_NAME,
	UNEXPECTED_DB_ERROR
} srv_ret_code;

srv_ret_code srv_write_product(int fromId, void * msg, int msgLen);
srv_ret_code srv_get_product(int fromId, void * msg, int msgLen);
srv_ret_code srv_remove_product(int fromId, void * msg, int msgLen);

#endif
