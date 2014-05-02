#ifndef _PRODUCT_DB_
#define _PRODUCT_DB_

#include "common.h"
#include "product.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h> // for perror and remove

#define DB_ROOT_PATH "so-db"
#define TABLE_NAME "product"

#define TABLE_PATH	DB_ROOT_PATH"/"TABLE_NAME
#define BUFFER_SIZE 100

typedef enum {
	DB_NOT_INITIALIZED = OK+1,
	CANNOT_CREATE_DATABASE,
	CANNOT_CREATE_TABLE,
	NO_PRODUCT_FOR_NAME,
	PRODUCT_EXISTS,
	UNEXPECTED_DELETE_ERROR,
	UNEXPECTED_UPDATE_ERROR
} db_ret_code;

typedef enum {
	READ_MODE, 
	WRITE_MODE,
	UNLOCK
} lock_mode;

typedef enum {
	SHOW,
	ADD,
	REMOVE,
	DEPOSIT,
	TAKE
} client_action;

typedef struct {
    short l_type;    /* Type of lock: F_RDLCK, F_WRLCK, F_UNLCK */
    short l_whence;  /* How to interpret l_start: SEEK_SET, SEEK_CUR, SEEK_END */
    off_t l_start;   /* Starting offset for lock */
    off_t l_len;     /* Number of bytes to lock */
    pid_t l_pid;     /* PID of process blocking our lock (F_GETLK only) */
} flock;

db_ret_code db_init();
db_ret_code db_save_product(Product product, client_action action);
db_ret_code db_get_product_by_name(product_name name, Product * productp,client_action action, FILE * file, flock * flptr);
db_ret_code db_update_product(Product product,client_action action);
db_ret_code db_delete_product(product_name name,client_action action);

#endif