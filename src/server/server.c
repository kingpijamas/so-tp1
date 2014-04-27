#include "../../include/server.h"

#include <stdio.h>
#include <unistd.h>

static void __handle_get_product(int client_id);
static void __handle_write_product(int client_id);
static void __handle_remove_product(int client_id);
static void __handle_invalid_call(int client_id);
static void __recv(void * buf, int len);
static void __send(int to_id, void * buf, int len);
static void __assert(int ret_status);

static char buf[MAX_MSG_LENGTH];

srv_ret_code srv_start() { //TODO: signal()!
	int from_id;
	msg_type type;

	__assert(ipc_init(SRV_ID));
	__assert(db_init());

	while(true) {
		printf("Srv: Sleeping...\n");
		usleep(700 * 1000);
		printf("Srv: ...Woke up\n");
		__recv(&from_id, sizeof(int));
		printf("Srv:\n\t< id = %d ...\n", recv);
		__recv(&type, sizeof(msg_type));
		printf("\ttype = %d ", type);
		switch(type) {
			case GET_PRODUCT:
				printf("(GET_PRODUCT) ");
				__handle_get_product(from_id);
				break;
			case WRITE_PRODUCT:
				printf("(WRITE_PRODUCT) ");
				__handle_write_product(from_id);
				break;
			case REMOVE_PRODUCT:
				printf("(REMOVE_PRODUCT) ");
				__handle_remove_product(from_id);
				break;
			default:
				__handle_invalid_call(from_id);
				break;
		}
	}
	//ipc_close(SRV_ID);
	return OK;
}

srv_ret_code srv_get_product(product_name name, Product * productp) {
	return db_get_product_by_name(name, productp);
}

srv_ret_code srv_write_product(Product product) {
	int ret;
	if ((ret = db_update_product(product)) == NO_PRODUCT_FOR_NAME) {
		ret = db_save_product(product);
	}
	return ret;
}

srv_ret_code srv_remove_product(product_name name) {
	return db_delete_product(name);
}

void __handle_get_product(int client_id) {
	int ret;
	product_name_msg msg;
	Product product;
	char resp[sizeof(product_resp)]; //maybe buffer could be used here

	__recv(&buf, sizeof(msg) - sizeof(msg_type));

	if ((ret = msg_deserialize_product_name_msg(buf, &msg)) == OK 
		&& (ret = srv_get_product(msg.name, &product)) == OK) {
		msg_serialize_product_resp(msg_product_resp_new(OK_RESP, product), resp);
		__send(client_id, (void *) resp, sizeof(product_resp));
	} else {
		msg_serialize_error_resp(msg_error_resp_new(ERR_RESP, ret), resp);
		__send(client_id, resp, sizeof(error_resp));
	}
}

void __handle_write_product(int client_id) {
	int ret;
	product_msg msg;
	Product product;
	char resp[sizeof(error_resp)]; //maybe buffer could be used here

	__recv(&buf, sizeof(product_msg) - sizeof(msg_type) - sizeof(int));

	if ((ret = msg_deserialize_product_msg(buf, &msg)) == OK 
		&& (ret = srv_write_product(product)) == OK) {
		msg_serialize_error_resp(msg_error_resp_new(OK_RESP, ret), resp);
		__send(client_id, (void *) resp, sizeof(error_resp));
	} else {
		msg_serialize_error_resp(msg_error_resp_new(ERR_RESP, ret), resp);
		__send(client_id, (void *) resp, sizeof(error_resp));
	}
}

void __handle_remove_product(int client_id) {
	int ret;
	product_name_msg msg;
	char resp[sizeof(error_resp)]; //maybe buffer could be used here

	__recv(&buf, sizeof(product_name_msg) - sizeof(msg_type) - sizeof(int));

	if ((ret = msg_deserialize_product_name_msg(buf, &msg)) == OK 
		&& (ret = srv_remove_product(msg.name)) == OK) {
		msg_serialize_error_resp(msg_error_resp_new(OK_RESP, ret), resp);
		__send(client_id, (void *) resp, sizeof(error_resp));
	} else {
		msg_serialize_error_resp(msg_error_resp_new(ERR_RESP, ret), resp);
		__send(client_id, (void *) resp, sizeof(error_resp));
	}
}

void __handle_invalid_call(int client_id) {
 	char resp[sizeof(error_resp)]; //maybe buffer could be used here

 	msg_error_resp_new(ERR_RESP, INVALID_MSG);
	__send(client_id, (void *) resp, sizeof(error_resp));
}

void __recv(void * buf, int len) {
	ipc_recv(SRV_ID, buf, len);	
}

void __send(int to_id, void * buf, int len) {
	ipc_send(SRV_ID, to_id, buf, len);	
}

void __assert(int ret_status) { //TODO: check
	if (ret_status != OK) {
		printf("Failed (%d)\n", ret_status);
		//exit(1);
	}
}
