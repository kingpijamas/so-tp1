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

#define SRV_RESP_LEN 2

static void fatal(char *s);
static void bye(int sig);
static void __send(void * buf, int len);
static void __recv(void * buf, int len);
static boolean __get_product(string name, boolean expecting_failure);
static int __remove_product(string name, boolean expecting_failure);
static int __write_product(string name, int quantity, boolean expecting_failure);

int main(int argc, char **argv) {
	boolean failed = false;
	Product a = product_new("pen", 100), b = product_new("stapler", 500);

	db_init();

	db_save_product(a);
	//db_save_product(b);

	switch ( fork() ) {
		case -1:
			fatal("Fork error");
			break;
		case 0: /* child */
			//printf("Clt: get_product...\n");
			/*failed = __get_product("pen", false);
			failed = __get_product("pen", false);
			failed = __remove_product("pen", false);
			failed = __get_product("pen", true);
			*/
//			failed = __write_product("rubber", 80, false);
			/*failed = __get_product("rubber", false);
			failed = __write_product("rubber", 1, false);
			failed = __get_product("rubber", false);*/
			//failed = __get_product("rubber", false);
			//failed = __get_product("rubber", false);
			//failed = __write_product("glue", 10, false);
			//failed = __get_product("glue", false);
//			failed = __remove_product("rubber", false);
//			failed = __get_product("rubber", true);
//			failed = __write_product("rubber", 1, false);
			/*failed = __write_product("rubber", -1, false);
			failed = __get_product("rubber", false);
			failed = __remove_product("rubber", false);
			failed = __get_product("rubber", true);
			*/

			failed = __write_product("rubber", 80, false);
			failed = __remove_product("rubber", false);
			failed = __get_product("rubber", true);
			failed = __write_product("rubber", 1, false);
			
			printf("\n\nAll tests done: %s\n", failed == true? "[FAILED]":"[OK]");
			ipc_close(INVALID);
			break;
		default: /* parent */
			signal(SIGPIPE, bye);
			srv_start();
			break;
	}
	return 0;
}

boolean __get_product(string name, boolean expecting_failure){
	char toSend[sizeof(product_name_msg)];
	msg_type type;
	char resp_body[sizeof(product_resp)];
	Product product;
	
	msg_serialize_product_name_msg(msg_product_name_msg_new(CLT_ID, GET_PRODUCT, name), toSend);
	__send(toSend, sizeof(product_name_msg));
	__recv(&type, sizeof(msg_type));
	switch (type) {
		case OK_RESP:
			__recv(resp_body, sizeof(Product));
			msg_deserialize_product(resp_body, &product);
			printf("Clt: ...Received -- Product: {name: %s, quantity: %d}) -- %s\n", product.name, product.quantity, expecting_failure == true? "[ERROR]":"[OK]");
			return expecting_failure == true;
		case ERR_RESP:
			printf("Clt: ...Received -- %s\n", expecting_failure == true? "[OK] (expected)":"[ERROR]");
			return expecting_failure == false;
		default:
			printf("[ERROR]\n");
			return true;
	}
}

int __remove_product(string name, boolean expecting_failure){
	char toSend[sizeof(product_name_msg)];
	msg_type type;
	char resp_body[sizeof(error_resp)];
	int code;
	
	msg_serialize_product_name_msg(msg_product_name_msg_new(CLT_ID, REMOVE_PRODUCT, name), toSend);
	__send(toSend, sizeof(product_name_msg));
	__recv(&type, sizeof(msg_type));
	switch (type) {
		case OK_RESP:
			__recv(resp_body, sizeof(int));
			msg_deserialize_code(resp_body, &code);
			printf("Clt: ...Received -- { code: %d }) -- %s\n", code, expecting_failure == true? "[ERROR]":"[OK]");
			return expecting_failure == true;
		case ERR_RESP:
			printf("Clt: ...Received -- %s\n", expecting_failure == true? "[OK] (expected)":"[ERROR]");
			return expecting_failure == false;
		default:
			printf("[ERROR]\n");
			return true;
	}
}

int __write_product(string name, int quantity, boolean expecting_failure){
	char toSend[sizeof(product_msg)];
	msg_type type;
	char resp_body[sizeof(error_resp)];
	int code;
	
	msg_serialize_product_msg(msg_product_msg_new(CLT_ID, WRITE_PRODUCT, product_new(name, quantity)), toSend);
	__send(toSend, sizeof(product_msg));
	__recv(&type, sizeof(msg_type));
	switch (type) {
		case OK_RESP:
			__recv(resp_body, sizeof(int));
			msg_deserialize_code(resp_body, &code);
			printf("Clt: ...Received -- { code: %d }) -- %s\n", code, expecting_failure == true? "[ERROR]":"[OK]");
			return expecting_failure == true;
		case ERR_RESP:
			printf("Clt: ...Received -- %s\n", expecting_failure == true? "[OK] (expected)":"[ERROR]");
			return expecting_failure == false;
		default:
			printf("[ERROR]\n");
			return true;
	}
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