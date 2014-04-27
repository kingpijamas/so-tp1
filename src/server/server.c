#include "../../include/server.h"
#include "../../include/utils.h"
#include <stdio.h>
#include <unistd.h>

static void __handle_get_product(int client_id);
static void __handle_write_product(int client_id);
static void __handle_remove_product(int client_id);
static void __handle_invalid_call(int client_id);
static void __recv(void * buf, int len);
static void __send(int to_id, void * buf, int len);
static void __assert(int ret_status);
static void __send_err_resp(int client_id, boolean status, int code);

srv_ret_code srv_start() { //TODO: signal()!
	int from_id;
	msg_type type;

	__assert(ipc_init(SRV_ID));
	__assert(db_init());

	while(true) {
		printf("Srv: Sleeping...\n");
		usleep(700 * 1000);
		printf("Srv: ...Woke up\n");
		printf("Srv: Reading caller_id\n");
		__recv(&from_id, sizeof(int));
		printf("Srv:\t< id = %d ...\n", from_id);
		printf("Srv: Reading msg_type\n");
		__recv(&type, sizeof(msg_type));
		printf("Srv:\t... type = %d ", type);
		switch(type) {
			case GET_PRODUCT:
				printf("(GET_PRODUCT) ...\n");
				__handle_get_product(from_id);
				printf("Srv:\t... >\n");
				break;
			case WRITE_PRODUCT:
				printf("(WRITE_PRODUCT) ...\n");
				__handle_write_product(from_id);
				printf("Srv:\t... >\n");
				break;
			case REMOVE_PRODUCT:
				printf("(REMOVE_PRODUCT) ...\n");
				__handle_remove_product(from_id);
				printf("Srv:\t... >\n");
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
	char msg[sizeof(product_name)];
	product_name name;
	Product product;
	char resp[max(sizeof(product_resp),sizeof(error_resp))]; //maybe buffer could be used here

	printf("Srv: Reading message body\n");
	__recv(&msg, sizeof(product_name));

	if ((ret = msg_deserialize_product_name(msg, name)) == OK 
		&& (ret = srv_get_product(name, &product)) == OK) {
		msg_serialize_product_resp(OK_RESP, product, resp);
		__send(client_id, resp, sizeof(product_resp));
		return;
	}
	__send_err_resp(client_id, ERR_RESP, ret);
}

void __handle_remove_product(int client_id) {
	int ret;
	char msg[sizeof(product_name)];
	product_name name;
	msg_type status = ERR_RESP;

	printf("Srv: Reading message body\n");
	__recv(&msg, sizeof(product_name));

	if ((ret = msg_deserialize_product_name(msg, name)) == OK 
		&& (ret = srv_remove_product(name)) == OK) {
		status = OK_RESP;
	}
	__send_err_resp(client_id, status, ret);
}


void __handle_write_product(int client_id) {
	int ret;
	char msg[sizeof(Product)];
	Product product;
	msg_type status=ERR_RESP;

	printf("Srv: Reading message body\n");
	__recv(&msg, sizeof(Product));

	if ((ret = msg_deserialize_product(msg, &product)) == OK 
		&& (ret = srv_write_product(product)) == OK) {
		status = OK_RESP;
	}
	__send_err_resp(client_id, status, ret);
}

void __send_err_resp(int client_id, boolean status, int code) {
	char resp[sizeof(error_resp)]; //maybe buffer could be used here
	msg_serialize_error_resp(status, code, resp);
	__send(client_id, resp, sizeof(error_resp));
}

void __handle_invalid_call(int client_id) {
 	__send_err_resp(client_id, ERR_RESP, INVALID_MSG);
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
