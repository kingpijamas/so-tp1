#include "../../include/productDB.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h> // for perror and remove

typedef struct {
    short l_type;    /* Type of lock: F_RDLCK, F_WRLCK, F_UNLCK */
    short l_whence;  /* How to interpret l_start: SEEK_SET, SEEK_CUR, SEEK_END */
    off_t l_start;   /* Starting offset for lock */
    off_t l_len;     /* Number of bytes to lock */
    pid_t l_pid;     /* PID of process blocking our lock (F_GETLK only) */
} flock;

static void __write_new(Product product);
static FILE * __open(string name, const string mode);
static string __get_path_to_tuple(string name);

static boolean init = false;
static char buf[BUFFER_SIZE];

static flock __init_flock();
static void __lock(int fd, flock * flptr);
static void __unlock(int fd, flock * flptr);


db_ret_code db_init() {
	if (init) { // maybe print that it was initialized several times, but it's entirely not critical
		return OK;
	}
	// read/write/search permissions for owner and group, and with read/search permissions for others
	if (mkdir(DB_ROOT_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_DATABASE;
	}
	if (mkdir(TABLE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_TABLE;
	}
	init = true;
	return OK;
}

db_ret_code db_save_product(Product product) {
	int getVal;
	if (!init) {
		return DB_NOT_INITIALIZED;
	}

	getVal = db_get_product_by_name(product.name, &product);
	switch (getVal) {
		case OK:
			return PRODUCT_EXISTS;
		case NO_PRODUCT_FOR_NAME:
			__write_new(product);
			return OK;
		default:
			return getVal;
	}
}

db_ret_code db_get_product_by_name(product_name name, Product * productp) {
	Product rdProduct;
	int fd;
	flock fl=__init_flock();
	fl.l_pid=getpid();
	flock * flptr=&fl;

	if (!init) {
		return DB_NOT_INITIALIZED;
	}	
	FILE * file = __open(name, "r");
	if (file == NULL) {
		return NO_PRODUCT_FOR_NAME;
	}

	fd=fileno(file);
	__lock(fd, flptr);

	product_set_name(&rdProduct, name);
	while(fscanf(file,"%d\n", &((rdProduct).quantity)) != EOF) {;}

	__unlock(fd, flptr);
	close(fd);
	*productp = rdProduct;
	return OK;
}


db_ret_code db_update_product(Product product) {
	int getVal;
	Product originalProduct;

	if (!init) {
		return DB_NOT_INITIALIZED;
	}
	getVal = db_get_product_by_name(product.name, &originalProduct);
	switch (getVal) {
		case NO_PRODUCT_FOR_NAME:
			return NO_PRODUCT_FOR_NAME;
		case OK:
			product_set_quantity(&product, product.quantity+originalProduct.quantity);
			__write_new(product);
			return OK;
		default:
			return UNEXPECTED_UPDATE_ERROR;
	}
}

db_ret_code db_delete_product(product_name name) {
	Product product;
	int getVal; 
	if (!init) {
		return DB_NOT_INITIALIZED;
	}

	getVal = db_get_product_by_name(name, &product);
	switch (getVal) {
		case OK:
			if (remove(__get_path_to_tuple(name)) != 0){
				return UNEXPECTED_DELETE_ERROR;
			}
			return OK;
		default:
			return getVal; // this should probably be UNEXPECTED_DELETE_ERROR
	}
}

void __write_new(Product product) {
	// int fd;
	// flock fl=__init_flock();
	// fl.l_pid=getpid();
	// flock * flptr=&fl;
	FILE * file = __open(product.name, "w");
	// fd=fileno(file);
	// __lock(fd,flptr);
	fprintf(file, "%d\n", product.quantity);
	// __unlock(fd,flptr);
	fclose(file);
}

FILE * __open(product_name name, const string mode) {
	return fopen(__get_path_to_tuple(name), mode);
}

string __get_path_to_tuple(product_name name) {
	sprintf(buf, "%s/%s", TABLE_PATH, name); //this should clear the buffer (verify!)
	return buf;
}

flock __init_flock(){
	/* flock = {l_type l_whence, l_start, l_len, l_pid} */
	flock fl={F_RDLCK,SEEK_SET,0,0,0};
	return fl;
}

int __change_lock(flock * flptr, int lock_mode){
	switch(lock_mode){
	case READ_MODE:
		flptr->l_type=F_RDLCK;
		return OK;
	case WRITE_MODE:
		flptr->l_type=F_WRLCK;
		return OK;
	default:
		return ERROR;
	}
}

void __lock(int fd, flock * flptr){
	// printf("Press a key to try to get lock: \n");
	// getchar();
	// printf("Trying to get lock...\n");

    //Getting lock. If there is another lock, it will stay here till it's free
	if(fcntl(fd,F_SETLKW,flptr)==-1){ 
		perror("fcntrl");
		exit(1);
	}
}

void __unlock(int fd, flock * flptr){
	flptr->l_type=F_UNLCK; /*set to unlock same region */
	if (fcntl(fd,F_SETLKW, flptr)==-1){ //release the lock
		perror("fcntrl");
		exit(1);
	}
	close(fd);	
}