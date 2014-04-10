#include "../include/dbx.h"

// move to locks.c
int dbx_change_lock(struct flock fl, int lock_mode){
	if(READ_MODE){
		fl.l_type=F_RDLCK;
		return OK;
	}else if(WRITE_MODE){
		fl.l_type=F_WRLCK;
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

 db_ret_code dbx_save_product(Product product);//write_mode
 db_ret_code dbx_update_product(Product product);//write_mode
 db_ret_code dbx_delete_product(char * name); //write_mode

 db_ret_code dbx_get_product_by_name(char * name, Product * product){
	int db_ret_code_dbx;
	int fd;
	
	struct flock fl=init_flock();
	fl.l_pid=getpid();
	
	Product dbxprod;
	dbxprod.name=malloc(MAX_NAME_DB); //Hacer un product init en commoc.c o algo así

	char fname[32];
	sprintf(fname, "%s/%s", TABLE_PATH, name);

	//Open to get fd. Is there another way of doing it?
	if((fd=open(fname, O_RDWR)) == -1){
		perror("open");
		exit(1);
	}

	printf("Press a key to try to get lock: ");
    getchar();
    printf("Trying to get lock...");

    //Getting lock. If there is another lock, it will stay here till it's free
	if(fcntl(fd,F_SETLKW,&fl)==-1){ 
		perror("fcntrl");
		exit(1);
	}

	printf("got lock\n");
    printf("Press a key to release lock: ");
    getchar();
	
	//Asks the DB
	db_ret_code_dbx=db_get_product_by_name("pen",&dbxprod);
	if(db_ret_code_dbx!=OK){
		return db_ret_code_dbx;
	}

	fl.l_type=F_UNLCK; /*set to unlock same region */
	if (fcntl(fd,F_SETLKW, &fl)==-1){ //release the lock
		perror("fcntrl");
		exit(1);
	}

	close(fd);
	return PRODUCT_EXISTS;
}