#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/msg.h"

static msg_ret_code __serialize_msg(int from_id, msg_type type, void * attr, int attr_type_size, void * buf);
static msg_ret_code __serialize_resp(msg_type type, void * attr, int attr_type_size, void * buf);

product_name_msg msg_product_name_msg_new(int from_id, msg_type type, product_name name) {
	product_name_msg msg;
	msg.from_id = from_id;
	msg.type = type;
	memcpy(msg.name, name, sizeof(product_name));
	return msg;
}

product_msg msg_product_msg_new(int from_id, msg_type type, Product product) {
	product_msg msg;
	msg.from_id = from_id;
	msg.type = type;
	msg.product = product;
	return msg;
}

product_resp msg_product_resp_new(msg_type type, Product product) {
	product_resp resp;
	resp.type = type;
	resp.product = product;
	return resp;
}

error_resp msg_error_resp_new(msg_type type, int code) {
	error_resp resp;
	resp.type = type;
	resp.code = code;
	return resp;
}

msg_ret_code msg_serialize_product_name_msg(product_name_msg msg, void * buf) {
	product_name_msg aux;
	int ret;

	//validar tipo
	printf("<msg_serialize_product_name_msg>\tfrom_id=%d type=%d name=%s\n", msg.from_id, msg.type, msg.name);
	
	ret = __serialize_msg(msg.from_id, msg.type, &msg.name, sizeof(product_name), buf);

	msg_deserialize_product_name_msg(buf, &aux);

	return ret;
	//return __serialize_msg(msg.from_id, msg.type, &msg.name, sizeof(product_name), buf);
}

msg_ret_code msg_serialize_product_msg(product_msg msg, void * buf) {
	printf("<msg_serialize_product_msg>\tfrom_id=%d type=%d { name=%s, quantity=%d }\n", msg.from_id, msg.type, msg.product.name, msg.product.quantity);
	return __serialize_msg(msg.from_id, msg.type, &msg.product, sizeof(Product), buf);
}

msg_ret_code msg_serialize_product_resp(product_resp resp, void * buf) {
	//validar tipo
	return __serialize_resp(resp.type, &resp.product, sizeof(Product), buf);
}

msg_ret_code msg_serialize_error_resp(error_resp resp, void * buf) {
	//validar tipo
	return __serialize_resp(resp.type, &resp.code, sizeof(int), buf);
}

msg_ret_code msg_deserialize_product(void * buf, Product * productp) {
	memcpy(productp, buf, sizeof(Product));
	return OK;
}

msg_ret_code msg_deserialize_product_name(void * buf, product_name name) {
	memcpy(name, buf, sizeof(product_name));
	return OK;
}

msg_ret_code msg_deserialize_code(void * buf, int * code) {
	memcpy(code, buf, sizeof(int));
	return OK;
}


msg_ret_code msg_deserialize_product_name_msg(void * buf, product_name_msg * msg) {
	printf("<msg_deserialize_product_name_msg>\t");
	memcpy(&(msg->from_id), buf, sizeof(int));
	printf("from_id=%d ", msg->from_id);
	memcpy(&(msg->type), buf+sizeof(int), sizeof(msg_type));
	printf("type=%d ", msg->type);
	memcpy(&(msg->name), buf+sizeof(int)+sizeof(msg_type), sizeof(product_name));
	printf("name=%s\n", msg->name);
	//msg = (product_name_msg *) buf;
	//memcpy?
	return OK;
}

msg_ret_code msg_deserialize_product_resp(void * buf, product_resp * resp) {
	printf("<msg_deserialize_product_resp>\t");
	memcpy(&(resp->type), buf, sizeof(msg_type));
	printf("type=%d ", resp->type);
	memcpy(&(resp->product), buf+sizeof(msg_type), sizeof(Product));
	printf("name={ name:%s, quantity:%d }\n", resp->product.name, resp->product.quantity);

	//resp = (product_resp *) buf;
	return OK;
}

/*msg_ret_code msg_deserialize_error_resp(void * buf, error_resp * resp) {
	printf("size: %d\n", sizeof(buf));
	resp = (error_resp *) buf;
	//memcpy?
	return OK;
}*/

msg_ret_code __serialize_msg(int from_id, msg_type type, void * attr, int attr_type_size, void * buf) {	
	memcpy(buf, &from_id, sizeof(int));
	memcpy(buf+sizeof(int), &type, sizeof(msg_type));
	memcpy(buf+sizeof(int)+sizeof(msg_type), attr, attr_type_size);
	return OK;
}

msg_ret_code __serialize_resp(msg_type type, void * attr, int attr_type_size, void * buf) {
	memcpy(buf, &type, sizeof(msg_type));
	memcpy(buf+sizeof(msg_type), attr, attr_type_size);
	return OK;
}
