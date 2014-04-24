#include server.h 

static void __assert(int ret_status);
static boolean running = false;


void __assert(int ret_status) { //TODO: check
	if (ret_status != OK) {
		printf("Failed (%d)\n", ret_status);
		exit(1);
	}
}

srv_ret_code srv_start() {
	if (running) {
		return OK;
	}
	__assert(ipc_init());
	__assert(db_init());
	
	

	running = true;
	return ;
}

srv_ret_code srv_write_product(int fromid, void * msg, int msglen) {
	
	db_save_product();
}

	db_ret_code db_save_product(Product product);
	db_ret_code db_get_product_by_name(string name, Product * productp);
	db_ret_code db_update_product(Product product);
	db_ret_code db_delete_product(string name);

srv_ret_code srv_get_product(int fromid, void * msg, int msglen) {

}

srv_ret_code srv_remove_product(int fromid, void * msg, int msglen) {

}
