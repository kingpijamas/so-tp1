#include "../include/dbx.h"

// move to locks.c
int dbx_change_lock(struct flock * fl, int lock_mode){
	if(READ_MODE){
		(*fl).l_type=F_RDLCK;
		return OK;
	}else if(WRITE_MODE){
		(*fl).l_type=F_WRLCK;
		return OK;
	}else{
		return ERROR;
	}
}

//move to lock.c
struct flock init_flock(){
	/* flock = {l_type l_whence, l_start, l_len, l_pid} */
	struct flock fl={F_RDLCK,SEEK_SET,0,0,0};
	return fl;
}

int open_and_lock(char * name, struct flock * fl){
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

void unlock_and_close(int fd, struct flock * fl){
	(*fl).l_type=F_UNLCK; /*set to unlock same region */
	if (fcntl(fd,F_SETLKW, fl)==-1){ //release the lock
		perror("fcntrl");
		exit(1);
	}

	close(fd);
}

//REVISAR... VALE LA PENA LOCKEAR? ALGUIN PUEDE ENTRAR?
//DEBERIA ESTAR EN EL MEDIO DE __WRITE_NEW
 db_ret_code dbx_save_product(Product product){
 	int db_ret_code;
	int fd;

	struct flock fl=init_flock();
	fl.l_pid=getpid();
	dbx_change_lock(&fl,WRITE_MODE);


	db_ret_code=db_save_product(product);

	fd=open_and_lock(product.name, &fl);

	unlock_and_close(fd, &fl);

	return db_ret_code;
 };

 db_ret_code dbx_update_product(Product product){
 	int db_ret_code;
	int fd;

	struct flock fl=init_flock();
	fl.l_pid=getpid();
	dbx_change_lock(&fl,WRITE_MODE);

	fd=open_and_lock(product.name, &fl);

	db_ret_code=db_update_product(product);

	unlock_and_close(fd, &fl);

	return db_ret_code;
 };

 db_ret_code dbx_delete_product(char * name){
 	int db_ret_code;
	int fd;

	struct flock fl=init_flock();
	fl.l_pid=getpid();
	dbx_change_lock(&fl,WRITE_MODE);

	fd=open_and_lock(name, &fl);

	db_ret_code=db_delete_product(name);

	unlock_and_close(fd, &fl);

	return db_ret_code;
 };

 db_ret_code dbx_get_product_by_name(char * name, Product * product){
	int db_ret_code;
	int fd;
	struct flock fl=init_flock();
	fl.l_pid=getpid();
	Product dbxprod;
	product_init(&dbxprod);

	fd=open_and_lock(name, &fl);

	//Asks the DB
	db_ret_code=db_get_product_by_name("pen",&dbxprod);

	(*product).name=dbxprod.name;
	(*product).quantity=dbxprod.quantity;

	unlock_and_close(fd, &fl);

	return db_ret_code;
}