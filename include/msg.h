#ifndef _MSG_
#define _MSG_

#include "common.h"
#include "product.h"

typedef enum {
	GET_PRODUCT,
	WRITE_PRODUCT,
	REMOVE_PRODUCT
} msg_type;

typedef struct {
	msg_type method;
	product_name name;
} named_product_msg;

typedef struct {
	msg_type method;
	Product product;
} write_product_msg;

typedef named_product_msg get_product_msg;
typedef named_product_msg remove_product_msg;

typedef enum {
	INVALID_METHOD = OK+1,
	INVALID_BODY
} msg_ret_code;

msg_ret_code msg_serialize_get_product_msg(get_product_msg msg, void * buf);
msg_ret_code msg_serialize_remove_product_msg(remove_product_msg msg, void * buf);
msg_ret_code msg_serialize_write_product_msg(write_product_msg msg, void * buf);
msg_ret_code msg_deserialize_msg_type(void * buf, msg_type * type);
msg_ret_code msg_deserialize_get_product_msg(void * buf, get_product_msg * msg);
msg_ret_code msg_deserialize_remove_product_msg(void * buf, remove_product_msg * msg);
msg_ret_code msg_deserialize_write_product_msg(void * buf, write_product_msg * msg);

#endif