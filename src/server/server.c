#include server.h 

#define SRV_ID 0
#define BUFFER_SIZE 

static void __assert(int ret_status);
static void __srv_rcv(void * buf, int len);
static boolean running = false;

static void buf[];

void __assert(int ret_status) { //TODO: check
	if (ret_status != OK) {
		printf("Failed (%d)\n", ret_status);
		exit(1);
	}
}

srv_ret_code srv_start() {
	srv_method method;

	if (running) {
		return OK;
	}
	__assert(ipc_init(SRV_ID));
	__assert(db_init());

	while(true) {
		usleep(700 * 1000);
		__srv_recv(&method, sizeof(srv_method));
		switch(method) {
			case GET_PRODUCT:
				__parse_product();
				__srv_recv( , sizeof(get_product_params));
				break;
			case WRITE_PRODUCT:
				__srv_recv( , sizeof(write_product_params));
				break;
			case REMOVE_PRODUCT:
				__srv_recv( , sizeof(remove_product_params));
				break;
		}
	}

	running = true;
	return ;
}

srv_ret_code srv_write_product(int fromid, Product product) {
	int ret;
	if ((ret = db_update_product(product)) == NO_PRODUCT_FOR_NAME) {
		ret = db_save_product(product);
	}

}

db_ret_code db_save_product(Product product);
db_ret_code db_get_product_by_name(string name, Product * productp);
db_ret_code db_update_product(Product product);
db_ret_code db_delete_product(string name);

srv_ret_code srv_get_product(int fromid, product_name name) {
	int ret;
	if ((ret = db_get_product_by_name(product)) == OK) {

	}
}

srv_ret_code srv_remove_product(int fromid, product_name ) {
	int ret;
	if (ret = db_delete_product()) {
		
	}
}

void __srv_rcv(void * buf, int len) {
	ipc_recv(SRV_ID, buf, len);	
}