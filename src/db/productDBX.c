#include "../../include/productDBX.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
    short l_type;    /* Type of lock: F_RDLCK, F_WRLCK, F_UNLCK */
    short l_whence;  /* How to interpret l_start: SEEK_SET, SEEK_CUR, SEEK_END */
    off_t l_start;   /* Starting offset for lock */
    off_t l_len;     /* Number of bytes to lock */
    pid_t l_pid;     /* PID of process blocking our lock (F_GETLK only) */
} flock;

static int __change_lock(flock * fl, int lock_mode);
static flock __init_flock();
static int __open_and_lock(string name, flock * fl);
static void __unlock_and_close(int fd, flock * fl);

//REVISAR... VALE LA PENA LOCKEAR? ALGUIN PUEDE ENTRAR?
//DEBERIA ESTAR EN EL MEDIO DE __WRITE_NEW
db_ret_code dbx_save_product(Product product){
 	int dbRet;
	int fd;
	flock fl=__init_flock();
	
	fl.l_pid=getpid();
	__change_lock(&fl,WRITE_MODE);

	dbRet=db_save_product(product);

	fd=__open_and_lock(product.name, &fl);

	__unlock_and_close(fd, &fl);

	return dbRet;
}

db_ret_code dbx_get_product_by_name(string name, Product * product){
	int dbRet;
	int fd;
	Product dbxprod;
	flock fl=__init_flock();

	fl.l_pid=getpid();

	fd=__open_and_lock(name, &fl);

	//Asks the DB
	dbRet=db_get_product_by_name("pen",&dbxprod);

	product->name=dbxprod.name;
	product->quantity=dbxprod.quantity;

	__unlock_and_close(fd, &fl);

	return dbRet;
}

db_ret_code dbx_update_product(Product product){
 	int dbRet;
	int fd;
	flock fl=__init_flock();

	fl.l_pid=getpid();
	__change_lock(&fl,WRITE_MODE);

	fd=__open_and_lock(product.name, &fl);

	dbRet=db_update_product(product);

	__unlock_and_close(fd, &fl);

	return dbRet;
}

db_ret_code dbx_delete_product(string name){
 	int dbRet;
	int fd;
	flock fl=__init_flock();

	fl.l_pid=getpid();
	__change_lock(&fl,WRITE_MODE);

	fd=__open_and_lock(name, &fl);

	dbRet=db_delete_product(name);

	__unlock_and_close(fd, &fl);

	return dbRet;
}

// move to locks.c
int __change_lock(flock * fl, int lock_mode){
	switch(lock_mode){
	case READ_MODE:
		fl->l_type=F_RDLCK;
		return OK;
	case WRITE_MODE:
		fl->l_type=F_WRLCK;
		return OK;
	default:
		return ERROR;
	}
}

//move to lock.c
flock __init_flock(){
	/* flock = {l_type l_whence, l_start, l_len, l_pid} */
	flock fl={F_RDLCK,SEEK_SET,0,0,0};
	return fl;
}

int __open_and_lock(string name, flock * fl){
	int fd;
	char fname[32];
	sprintf(fname, "%s/%s", TABLE_PATH, name);
	printf("%s\n",fname);

	//Open and get fd
	if((fd=open(fname, O_RDWR)) == -1){
		perror("open");
		exit(1);
	}

	printf("Press a key to try to get lock: ");
    getchar();
    printf("Trying to get lock...");

    //Getting lock. If there is another lock, it will stay here till it's free
	if(fcntl(fd,F_SETLKW,fl)==-1){ 
		perror("fcntrl");
		exit(1);
	}

	printf("got lock\n");
    printf("Press a key to release lock: ");
    getchar();
    return fd;
}

void __unlock_and_close(int fd, flock * fl){
	fl->l_type=F_UNLCK; /*set to unlock same region */
	if (fcntl(fd,F_SETLKW, fl)==-1){ //release the lock
		perror("fcntrl");
		exit(1);
	}
	close(fd);
}