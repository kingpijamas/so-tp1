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

#define CLT_ID 15
#define PROD_NAME "pencil"
#define INVALID -1

#define SRV_RESP_LEN 2

static void __assert(boolean failed, boolean expecting_failure);

int main(int argc, char **argv) {
	int from_id;
	int type;
	boolean failed = false;
	product_name_msg msg;
	char buf[sizeof(product_name_msg)];

//	GET: product_name_msg -> product_resp
	for (type=GET_PRODUCT; type<ERR_RESP; type++) {
		__assert(msg_product_name_msg(CLT_ID, type, PROD_NAME), false);
		__assert(msg_serialize_product_name_msg(msg, buf), false);
		__assert(msg_);
		__assert(msg_deserialize_product_name())
	}

//	REMOVE: product_name_msg -> error_resp
//	WRITE: product_msg -> error_resp

	return 0;
}

void __assert(boolean failed, boolean expecting_failure) {
		printf("[ERROR]");
	if (failed == expecting_failure) {
		printf(" (Expected)\n");
		return;
	}
	printf("\n");
	exit(1);
}


product_name_msg msg_product_name_msg_new(int from_id, msg_type type, product_name name);
product_msg msg_product_msg_new(int from_id, msg_type type, Product product);
product_resp msg_product_resp_new(msg_type type, Product product);
error_resp msg_error_resp_new(msg_type type, int code);

msg_ret_code msg_serialize_product_name_msg(product_name_msg msg, void * buf);
msg_ret_code msg_serialize_product_msg(product_msg msg, void * buf);
msg_ret_code msg_serialize_product_resp(product_resp msg, void * buf);
msg_ret_code msg_serialize_error_resp(error_resp resp, void * buf);

msg_ret_code msg_deserialize_product(void * buf, Product * productp);
msg_ret_code msg_deserialize_product_name(void * buf, product_name name);
msg_ret_code msg_deserialize_code(void * buf, int * code);

msg_ret_code msg_deserialize_product_name_msg(void * buf, product_name_msg * msg);
//msg_ret_code msg_deserialize_product_msg(void * buf, product_msg * msg);
msg_ret_code msg_deserialize_product_resp(void * buf, product_resp * resp);
msg_ret_code msg_deserialize_error_resp(void * buf, error_resp * resp);

#endif