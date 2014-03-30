#ifndef LOCKS_H
#define LOCKS_H

typedef struct {
    short l_type;    /* Type of lock: F_RDLCK,
                        F_WRLCK, F_UNLCK */
    short l_whence;  /* How to interpret l_start:
                        SEEK_SET, SEEK_CUR, SEEK_END */
    off_t l_start;   /* Starting offset for lock */
    off_t l_len;     /* Number of bytes to lock */
    pid_t l_pid;     /* PID of process blocking our lock
                        (F_GETLK only) */
} flock;

// void getLock(){
// 	l_whence = SEEK_SET
// 	l_start = 0
// 	l_len = // el tamaño del archivo
// 	l_type = 
// }

getpid()

#endif
