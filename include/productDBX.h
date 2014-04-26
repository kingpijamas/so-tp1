#ifndef _PRODUCT_DBX_
#define _PRODUCT_DBX_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.h"

typedef enum {
	READ_MODE, 
	WRITE_MODE
} lock_mode;

typedef struct {
    short l_type;    /* Type of lock: F_RDLCK, F_WRLCK, F_UNLCK */
    short l_whence;  /* How to interpret l_start: SEEK_SET, SEEK_CUR, SEEK_END */
    off_t l_start;   /* Starting offset for lock */
    off_t l_len;     /* Number of bytes to lock */
    pid_t l_pid;     /* PID of process blocking our lock (F_GETLK only) */
} flock;

int __change_lock(flock * fl, int lock_mode);
flock __init_flock();
void __lock(string name, flock * fl, int fd);
void __unlock(int fd, flock * fl);


 #endif