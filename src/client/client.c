#include "../../include/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../include/common.h"
#include "../../include/productDB.h"
#include "../../include/utils.h"
#include "../../include/communicator.h"
#include "../../include/server.h"
#include "../../include/msg.h"
#include <sys/types.h>
#include <unistd.h>

static int __get_product(product_name name, Product * product);
static int __remove_product(product_name name);
static int __write_product(product_name name, int quantity);
static int __handle_not_ok_resp(msg_type resp_type);
static int __get_id();
static void __connect();
static void __send(void * buf, int len);
static void __recv(void * buf, int len);
static void __disconnect();

// displays available data for product of name == prodname
clt_ret_code clt_show_product(product_name name){
	int ret;
	Product product;

	if ((ret = __get_product(name, &product)) == OK) {
		printf("{ name=%s, quantity=%d }\n", product.name, product.quantity);
	}
	return ret;
}

// adds a new product to the stock - this is used as an initialization method
clt_ret_code clt_add_product(product_name name, int quantity){
	Product product;
	if (__get_product(name, &product) == OK) {
		return PRODUCT_ALREADY_INIT;
	}
	return __write_product(name, quantity);
}

// removes a product from the stock altogether - the opposite of clt_add_product
clt_ret_code clt_remove_product(product_name name){
	return __remove_product(name);
}

// adds 'quantity' products with name == prodname
clt_ret_code clt_deposit_product(product_name name, int quantity){
	if (quantity < 0) {
		return NEGATIVE_QUANTITY;
	}
	return __write_product(name, quantity);
}

// removes 'quantity' in products with name == prodname - the opposite of clt_deposit_product
clt_ret_code clt_take_product(product_name name, int quantity){
	if (quantity < 0) {
		return NEGATIVE_QUANTITY;
	}
	return __write_product(name, -quantity);
}

int __get_product(product_name name, Product * productp){
	char toSend[sizeof(product_name_msg)];
	msg_type resp_type;
	char resp_body[sizeof(product_resp)];
	
	__connect();
	msg_serialize_product_name_msg(__get_id(), GET_PRODUCT, name, toSend);
	__send(toSend, sizeof(product_name_msg));
	__recv(&resp_type, sizeof(msg_type));

	if (resp_type == OK_RESP) {
		__recv(resp_body, sizeof(Product));
		msg_deserialize_product(resp_body, productp);
		__disconnect();
		return OK;
	}
	return __handle_not_ok_resp(resp_type);
}

int __remove_product(product_name name){
	char toSend[sizeof(product_name_msg)];
	msg_type resp_type;
	char resp_body[sizeof(error_resp)];
	int code;
	
	__connect();
	msg_serialize_product_name_msg(__get_id(), REMOVE_PRODUCT, name, toSend);
	__send(toSend, sizeof(product_name_msg));
	__recv(&resp_type, sizeof(msg_type));

	if (resp_type == OK_RESP) {
		__recv(resp_body, sizeof(int));
		msg_deserialize_code(resp_body, &code);
		__disconnect();
		return OK;
	}
	return __handle_not_ok_resp(resp_type);
}

int __write_product(product_name name, int quantity){
	char toSend[sizeof(product_msg)];
	msg_type resp_type;
	char resp_body[sizeof(error_resp)];
	int code;

	__connect();	
	msg_serialize_product_msg(__get_id(), WRITE_PRODUCT, product_new(name, quantity), toSend);
	__send(toSend, sizeof(product_msg));
	__recv(&resp_type, sizeof(msg_type));
	if (resp_type == OK_RESP) {
		__recv(resp_body, sizeof(int));
		msg_deserialize_code(resp_body, &code);
		__disconnect();
		return OK;
	}
	return __handle_not_ok_resp(resp_type);
}

int __handle_not_ok_resp(msg_type resp_type) {
	char resp_body[sizeof(int)];
	int code;
	__recv(resp_body, sizeof(int));
	msg_deserialize_code(resp_body, &code);
	__disconnect();
	switch(resp_type) {
		case ERR_RESP:
			//TODO: check!
			return code;
		default: //Should never happen
			printf("Clt: critical error. Cannot recover.\n");
			exit(1);
			return false;
	}
}

void __connect() {
	if (ipc_connect(__get_id(), SRV_ID) == OK) {
		printf("Clt: connected to srv\n");
	}
	printf("Clt: could not connect to srv [ERROR]\n");
}

void __disconnect() {
	if (ipc_disconnect(__get_id(), SRV_ID) == OK) {
		printf("Clt: diconnected from srv\n");
	}
	printf("Clt: could not diconnect from srv [ERROR]\n");
}

void __send(void * buf, int len) {
	printf("Clt: wrote %d bytes of %d\n", ipc_send(__get_id(), SRV_ID, buf, len), len);	
}

void __recv(void * buf, int len) {
	printf("Clt: read %d bytes of %d\n", ipc_recv(__get_id(), buf, len), len);
}

int __get_id() { //TODO: Check!
	return getpid();
}
