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

#define CLT_ID 1
#define INVALID -1

#define OK_MSG "OK"
#define NOT_OK_MSG "NK"
#define SRV_RESP_LEN 2

static void fatal(char *s);
static void bye(int sig);
static void __send(void * buf, int len);
static void __recv(void * buf, int len);
static boolean __get_product(string name, boolean expecting_failure);
static int __remove_product(string name);
static int __write_product(string name, int quantity);

int main(int argc, char **argv) {
	boolean failed = false;
	Product a = product_new("pen", 100), b = product_new("stapler", 500), c = product_new("rubber", 80);

	db_init();

	db_save_product(a);
	db_save_product(b);

	switch ( fork() ) {
		case -1:
			fatal("Fork error");
			break;
		case 0: /* child */
			printf("Clt: get_product...\n");
			failed = __get_product("pen", false);
			printf("Clt: Done. %s\n", failed? "[FAILED]":"All tests [OK]");
			break;
		default: /* parent */
			signal(SIGPIPE, bye);
			srv_start();
			ipc_close(INVALID);
			break;
	}
	return 0;
}

boolean __get_product(string name, boolean expecting_failure){
	char toSend[sizeof(product_name_msg)];
	msg_type type;
	char buf[sizeof(product_resp)];
	product_resp received;

	msg_serialize_product_name_msg(CLT_ID, msg_product_name_msg_new(CLT_ID, GET_PRODUCT, name), toSend);
	__send(toSend, sizeof(product_name_msg));
	
	__recv(&type, sizeof(msg_type));
	switch (type) {
		case OK_RESP:
			__recv(buf, sizeof(product_resp));
			msg_deserialize_product_resp(buf, &received);
			printf("Clt: ...Received -- Product: {name: %s, quantity: %d}) -- %s\n", received.product.name, received.product.quantity, expecting_failure? "[ERROR]":"[OK]");
			return !expecting_failure;
		default:
			printf("Clt: ...Received -- Invalid msg_type (%d) -- %s\n", type, expecting_failure? "[OK] (expected)":"[ERROR]");
			return expecting_failure;
	}
}

int __remove_product(string name){
	char buf[sizeof(product_name_msg)];
	msg_serialize_product_name_msg(CLT_ID, msg_product_name_msg_new(CLT_ID, REMOVE_PRODUCT, name), buf);
	return ipc_send(CLT_ID, SRV_ID, buf, sizeof(product_name_msg));
}

int __write_product(string name, int quantity){
	char buf[sizeof(product_msg)];
	msg_serialize_product_msg(CLT_ID, msg_product_msg_new(CLT_ID, WRITE_PRODUCT, product_new(name, quantity)), buf);
	return ipc_send(CLT_ID, SRV_ID, buf, sizeof(product_msg));
}


void fatal(char *s) {
	perror(s);
	exit(1);
}

void bye(int sig) {
	printf("Parent received SIGPIPE\n");
	exit(1);
}

void __send(void * buf, int len) {
	printf("Clt: wrote %d bytes of %d\n", ipc_send(CLT_ID, SRV_ID, buf, len), len);	
}

void __recv(void * buf, int len) {
	printf("Clt: read %d bytes of %d\n", ipc_recv(CLT_ID, buf, len), len);
}