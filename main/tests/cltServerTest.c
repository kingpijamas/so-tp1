#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../include/common.h"
#include "../../include/error.h"
#include "../../include/productDB.h"
#include "../../include/utils.h"
#include "../../include/communicator.h"
#include "../../include/server.h"
#include "../../include/msg.h"
#include "../../include/client.h"


#define CLT_ID 1
#define INVALID_ID -1

#define SRV_RESP_LEN 2

static void fatal(char *s);

int main(int argc, char **argv) {
	boolean passed;
	Product a = product_new("pen", 100)/*, b = product_new("stapler", 500)*/;

	db_init();
	db_save_product(a);
	//db_save_product(b);

	switch ( fork() ) {
		case -1:
			fatal("Fork error");
			break;
		case 0: /* child (client) */
			usleep(1000);
			passed = (clt_deposit_product("rubber", 80) == OK)
					&& (clt_remove_product("rubber") == OK)
					&& (clt_show_product("rubber") != OK)
					&& (clt_deposit_product("rubber", 1) == OK);
			printf("\n\nAll tests done: %s\n", !passed? "[FAILED]":"[OK]");
			//ipc_close(CLT_ID);
			break;
		default: /* parent (server) */
			srv_start();
			break;
	}
	return 0;
}

void fatal(char *s) {
	perror(s);
	exit(1);
}