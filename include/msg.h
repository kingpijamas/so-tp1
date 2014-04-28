#ifndef _MSG_
#define _MSG_

#include "common.h"
#include "product.h"

#define MAX_MSG_LENGTH 200

typedef enum {
	GET_PRODUCT,
	WRITE_PRODUCT,
	REMOVE_PRODUCT,
	OK_RESP,
	ERR_RESP
} msg_type;

typedef struct {
	int from_id;
	msg_type type;
	product_name name;
} product_name_msg;

typedef struct {
	int from_id;
	msg_type type;
	Product product;
} product_msg;

typedef struct {
	msg_type type;
	int code;
} error_resp;

typedef struct {
	msg_type type;
	Product product;
} product_resp;

typedef enum {
	INVALID_METHOD = OK+1,
	INVALID_BODY
} msg_ret_code;


msg_ret_code msg_serialize_product_name_msg(int from_id, msg_type type, product_name name, void * buf);
msg_ret_code msg_serialize_product_msg(int from_id, msg_type type, Product product, void * buf);
msg_ret_code msg_serialize_product_resp(msg_type type, Product product, void * buf);
msg_ret_code msg_serialize_error_resp(msg_type type, int code, void * buf);
msg_ret_code msg_deserialize_product(void * buf, Product * product);
msg_ret_code msg_deserialize_product_name(void * buf, product_name name);
msg_ret_code msg_deserialize_code(void * buf, int * code);

#endif