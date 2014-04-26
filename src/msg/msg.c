#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/msg.h"

static int __serialize(void * x, int x_type_size, void * buf);
static msg_ret_code __serialize_named_product_msg(named_product_msg msg, void * buf);
static msg_ret_code __deserialize_named_product_msg(void * buf, named_product_msg * msg);

msg_ret_code msg_serialize_get_product_msg(get_product_msg msg, void * buf) {
	return __serialize_named_product_msg(msg, buf);
}

msg_ret_code msg_serialize_remove_product_msg(remove_product_msg msg, void * buf) {
	return __serialize_named_product_msg(msg, buf);
}

msg_ret_code msg_serialize_write_product_msg(write_product_msg msg, void * buf) {
	__serialize(&msg.method, sizeof(msg_type), buf);
	__serialize(&msg.product, sizeof(product_name), buf+sizeof(product_name));
	return OK;
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

msg_ret_code msg_deserialize_get_product_msg(void * buf, get_product_msg * msg) {
	return __deserialize_named_product_msg(buf, msg);
}

msg_ret_code msg_deserialize_remove_product_msg(void * buf, remove_product_msg * msg) {
	return __deserialize_named_product_msg(buf, msg);
}

msg_ret_code msg_deserialize_write_product_msg(void * buf, write_product_msg * msg) {
	printf("size: %d\n", sizeof(buf));
	msg = (write_product_msg *) buf;
	//memcpy?
	return OK;
}

int __serialize(void * x, int x_type_size, void * buf) {
	memcpy(x, buf, x_type_size);
	return x_type_size;
}

msg_ret_code __serialize_named_product_msg(named_product_msg msg, void * buf) {
	__serialize(&msg.method, sizeof(msg_type), buf);
	__serialize(&msg.name, sizeof(product_name), buf+sizeof(product_name));
	return OK;
}

msg_ret_code __deserialize_named_product_msg(void * buf, named_product_msg * msg) {
	printf("size: %d\n", sizeof(buf));
	msg = (named_product_msg *) buf;
	//memcpy?
	return OK;
}