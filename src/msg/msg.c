#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/msg.h"

static msg_ret_code __serialize_msg(int from_id, msg_type type, void * attr, int attr_type_size, void * buf);
static msg_ret_code __serialize_resp(msg_type type, void * attr, int attr_type_size, void * buf);
static void __print_msg_type(msg_type type);

msg_ret_code msg_serialize_product_name_msg(int from_id, msg_type type, product_name name, void * buf) {
	printf("Serializing msg { from_id: %d, type: ", from_id);
	__print_msg_type(type);
	printf("(%d), name: %s }\n", type, name);
	return __serialize_msg(from_id, type, name, sizeof(product_name), buf);
}

msg_ret_code msg_serialize_product_msg(int from_id, msg_type type, Product product, void * buf) {
	printf("Serializing msg { from_id: %d, type:", from_id);
	__print_msg_type(type);
	printf("(%d), { name: %s, quantity: %d } }\n", type, product.name, product.quantity);
	return __serialize_msg(from_id, type, &product, sizeof(Product), buf);
}

msg_ret_code msg_serialize_product_resp(msg_type type, Product product, void * buf) {
	printf("Serializing resp { type: ");
	__print_msg_type(type);
	printf("(%d), { name: %s, quantity: %d } }\n", type, product.name, product.quantity);
	return __serialize_resp(type, &product, sizeof(Product), buf);
}

msg_ret_code msg_serialize_error_resp(msg_type type, int code, void * buf) {
	printf("Serializing resp { type: ");
	__print_msg_type(type);
	printf(", code: %d }\n", code);
	return __serialize_resp(type, &code, sizeof(int), buf);
}

msg_ret_code msg_deserialize_product(void * buf, Product * product) {
	//*product = ((Product *) buf)[0];
	memcpy(product, buf, sizeof(Product));
	printf("Deserialized { name: %s, quantity: %d }\n", product->name, product->quantity);
	return OK;
}

msg_ret_code msg_deserialize_product_name(void * buf, product_name name) {
	memcpy(name, buf, sizeof(product_name));
	printf("Deserialized { name: %s }\n", name);
	return OK;
}

msg_ret_code msg_deserialize_code(void * buf, int * code) {
	memcpy(code, buf, sizeof(int));
	printf("Deserialized { code: ");
	__print_msg_type(*code);
	printf(" (%d) }\n", *code);
	return OK;
}

msg_ret_code __serialize_msg(int from_id, msg_type type, void * attr, int attr_type_size, void * buf) {	
	memcpy(buf, &from_id, sizeof(int));
	//memcpy(buf+sizeof(int), &type, sizeof(msg_type));
	//memcpy(buf+sizeof(int)+sizeof(msg_type), attr, attr_type_size);
	return __serialize_resp(type, attr, attr_type_size, buf+sizeof(int));
}

msg_ret_code __serialize_resp(msg_type type, void * attr, int attr_type_size, void * buf) {
	memcpy(buf, &type, sizeof(msg_type));
	memcpy(buf+sizeof(msg_type), attr, attr_type_size);
	return OK;
}

void __print_msg_type(msg_type type) {
	switch (type) {
	case GET_PRODUCT:
		printf("GET_PRODUCT");
		return;
	case WRITE_PRODUCT:
		printf("WRITE_PRODUCT");
		return;
	case REMOVE_PRODUCT:
		printf("REMOVE_PRODUCT");
		return;
	case OK_RESP:
		printf("OK_RESP");
		return;
	case ERR_RESP:
		printf("ERR_RESP");
		return;
	}
}