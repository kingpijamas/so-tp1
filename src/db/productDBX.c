#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../include/productDBX.h"

// static int __change_lock(flock * fl, int lock_mode);
// static flock __init_flock();
// static int __open_and_lock(string name, flock * fl);
// static void __unlock_and_close(int fd, flock * fl);

//REVISAR... VALE LA PENA LOCKEAR? ALGUIN PUEDE ENTRAR?
//DEBERIA ESTAR EN EL MEDIO DE __WRITE_NEW
// db_ret_code dbx_save_product(Product product){
//  	int dbRet;
// 	int fd;
// 	flock fl=__init_flock();
	
// 	fl.l_pid=getpid();
// 	__change_lock(&fl,WRITE_MODE);

// 	dbRet=db_save_product(product);

// 	fd=__open_and_lock(product.name, &fl);

// 	__unlock_and_close(fd, &fl);

// 	return dbRet;
// }

// db_ret_code dbx_get_product_by_name(string name, Product * product){
// 	int dbRet;
// 	int fd;
// 	Product dbxprod;
// 	flock fl=__init_flock();

// 	fl.l_pid=getpid();
	
// 	product_init(&dbxprod);

// 	fd=__open_and_lock(name, &fl);

// 	//Asks the DB
// 	dbRet=db_get_product_by_name("pen",&dbxprod);

// 	product->name=dbxprod.name;
// 	product->quantity=dbxprod.quantity;

// 	__unlock_and_close(fd, &fl);

// 	return dbRet;
// }

// db_ret_code dbx_update_product(Product product){
//  	int dbRet;
// 	int fd;
// 	flock fl=__init_flock();

// 	fl.l_pid=getpid();
// 	__change_lock(&fl,WRITE_MODE);

// 	fd=__open_and_lock(product.name, &fl);

// 	dbRet=db_update_product(product);

// 	__unlock_and_close(fd, &fl);

// 	return dbRet;
// }

// db_ret_code dbx_delete_product(string name){
//  	int dbRet;
// 	int fd;
// 	flock fl=__init_flock();

// 	fl.l_pid=getpid();
// 	__change_lock(&fl,WRITE_MODE);

// 	fd=__open_and_lock(name, &fl);

// 	dbRet=db_delete_product(name);

// 	__unlock_and_close(fd, &fl);

// 	return dbRet;
// }

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

 void __lock(string name, flock * fl, int fd){
	// char fname[32];
	// sprintf(fname, "%s/%s", TABLE_PATH, name);
	// printf("%s\n",fname);

	printf("Press a key to try to get lock: ");
    getchar();
    printf("Trying to get lock...");

    //Getting lock. If there is another lock, it will stay here till it's free
	if(fcntl(fd,F_SETLKW,fl)==-1){ 
		perror("fcntrl");
		exit(1);
	}
	printf("got lock\n");

}

void __unlock(int fd, flock * fl){
	printf("Press a key to release lock: ");
    getchar();

	fl->l_type=F_UNLCK; /*set to unlock same region */
	if (fcntl(fd,F_SETLKW, fl)==-1){ //release the lock
		perror("fcntrl");
		exit(1);
	}
}