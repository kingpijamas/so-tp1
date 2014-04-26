#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/msg.h"

msg_ret_code __serialize_msg(msg_type type, void * attr, int attr_type_size void * buf);

msg_ret_code msg_serialize_product_name_msg(product_name_msg msg, void * buf) {
	//validar tipo
	return __serialize_msg(&msg.name, sizeof(product_name), buf);
}

msg_ret_code msg_serialize_product_msg(product_msg msg, void * buf) {
	//validar tipo
	return __serialize_msg(&msg.product, sizeof(product_name), buf);
}

msg_ret_code msg_serialize_product_resp(product_resp resp, void * buf) {
	//validar tipo
	return msg_serialize_product_msg(resp, buf);
}

msg_ret_code msg_serialize_error_resp(error_resp msg, void * buf) {
	//validar tipo
	return __serialize(&msg.product, sizeof(int), buf);
}

product_msg msg_product_msg_new(msg_type type, Product product) {
	product_msg msg;
	msg.type = type;
	msg.product = product;
	return msg_product_msg_new(type, msg);
}

product_resp msg_product_resp_new(msg_type type, Product product) {
	return msg_product_msg_new(type, product);
}

error_resp msg_error_resp_new(msg_type type, int code) {
	error_resp resp;
	resp.type = type;
	resp.code = code;
	return resp;
}

msg_ret_code msg_deserialize_msg_type(void * buf, msg_type * type) {
	*type = ((msg_type *) buf)[0];
	//memcpy(buf, type, sizeof(msg_type));
	switch(*type) {
		case GET_PRODUCT:
		case WRITE_PRODUCT:
		case REMOVE_PRODUCT:
			return OK;
		default:
			type = NULL;
			return INVALID_METHOD;
	}
}

msg_ret_code msg_deserialize_product_name_msg(void * buf, product_name_msg * msg) {
	printf("size: %d\n", sizeof(buf));
	msg = (product_name_msg *) buf;
	//memcpy?
	return OK;
}

msg_ret_code msg_deserialize_product_msg(void * buf, product_msg * msg) {
	printf("size: %d\n", sizeof(buf));
	msg = (product_msg *) buf;
	//memcpy?
	return OK;
}

msg_ret_code msg_deserialize_product_resp(void * buf, product_resp * resp) {
	return msg_deserialize_product_msg(buf, resp);
}

msg_ret_code msg_deserialize_error_resp(void * buf, error_resp * resp) {
	printf("size: %d\n", sizeof(buf));
	resp = (error_resp *) buf;
	//memcpy?
	return OK;
}

msg_ret_code __serialize_msg(msg_type type, void * attr, int attr_type_size void * buf) {
	memcpy(type, buf, sizeof(msg_type));
	memcpy(attr, buf+sizeof(msg_type), attr_type_size);
	return OK;
}
