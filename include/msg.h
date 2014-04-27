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

/*
product_name_msg msg_product_name_msg_new(int from_id, msg_type type, product_name name);
product_msg msg_product_msg_new(int from_id, msg_type type, Product product);
product_resp msg_product_resp_new(msg_type type, Product product);
error_resp msg_error_resp_new(msg_type type, int code);

msg_ret_code msg_serialize_product_name_msg(product_name_msg msg, void * buf);
msg_ret_code msg_serialize_product_msg(product_msg msg, void * buf);
msg_ret_code msg_serialize_product_resp(product_resp msg, void * buf);
msg_ret_code msg_serialize_error_resp(error_resp resp, void * buf);

msg_ret_code msg_deserialize_id(void * buf, );
msg_ret_code msg_deserialize_product(void * buf, Product * productp);
//if you're wondering why this (below) isn't a product_name *, it's because C changes 
//fixed size arrays (like product_name) to char * when they are used as parameters
msg_ret_code msg_deserialize_product_name(void * buf, product_name name);
msg_ret_code msg_deserialize_code(void * buf, int * code);


msg_ret_code msg_deserialize_product_name_msg(void * buf, product_name_msg * msg);
//msg_ret_code msg_deserialize_product_msg(void * buf, product_msg * msg);
msg_ret_code msg_deserialize_product_resp(void * buf, product_resp * resp);
msg_ret_code msg_deserialize_error_resp(void * buf, error_resp * resp);
*/

#endif