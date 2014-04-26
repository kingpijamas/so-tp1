#ifndef _SERVER_
#define _SERVER_

#include "common.h"
#include "product.h"
#include "communicator.h"
#include "productDB.h"
#include "msg.h"

typedef enum { 
	INVALID_MSG = OK+1,
} srv_ret_code;

srv_ret_code srv_start();
srv_ret_code srv_get_product(int fromid, void * msg, int msglen);
srv_ret_code srv_write_product(int fromid, void * msg, int msglen);
srv_ret_code srv_remove_product(int fromid, void * msg, int msglen);

#endif
