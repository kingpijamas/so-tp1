#include "../../include/communicator.h"
#include <string.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/common.h"
#include "../../include/semaphore.h"

#define MEM_KEY ((key_t) 0x111222)
#define SHM_SIZE 500

static void __assert(int res, string error_text);
static boolean __failed(int res);
static void __fail(string error_text);

static int shm_id, to_read=0, read=0;
static char * shm;

int ipc_init(int from_id) {
	__assert(shm_id = shmget(MEM_KEY, SHM_SIZE, IPC_CREAT /*| IPC_EXCL*/ | 0666), "Could not allocate memory");
	if ((shm = (char*)shmat(shm_id, NULL, 0)) == (void *)-1) {
		__fail("Could not allocate memory");
	}
	memset(shm, '\0', SHM_SIZE);
	semaphore_create();
	return OK;
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	semaphore_up();
	memcpy(shm, &len, sizeof(int));
	memcpy(shm+sizeof(int), buf, len);
	printf("wrote: %d\n", ((int *)shm)[0]);
	printf("%s: Writing %d bytes to %s\n", from_id==0? "Srv":"Clt", len, to_id==0? "Srv":"Clt");
	return len;
}

int ipc_recv(int from_id, void * buf, int len) {
	//int aux;
	printf("%s: Reading %d bytes\n", from_id==0? "Srv":"Clt", len);
	if (to_read - read < 0) {
		__fail("Negative amount to read");
	}
	if (to_read == read) {
		to_read = read = 0; //done reading
		semaphore_down();   //wait until there's more to read
		memcpy(&to_read, buf, sizeof(int));
	}
	printf("read: %d, len: %d, to_read: %d\n", read, len, to_read);
	if (read + len </*=?*/ to_read) {
		memcpy(buf, shm + sizeof(int) + read, len);
		read+=len;
		return len;
	}
	__fail("Reading overflow");
	//return to_read - (read + len);
	//aux = read + len - to_read;
	//memcpy(buf, shm + sizeof(int) + read, aux);
	//to_read = read = 0; //done reading
	//memcpy(buf, shm + sizeof(int) + read, len);
	//memset(shm, '\0', SHM_SIZE);
	return !OK;
}

int ipc_close(int from_id) {
	semaphore_destroy();
	shmdt(shm);
	shmctl(shm_id, IPC_RMID, 0);
	return OK;
}

void __assert(int res, string error_text) {
	int errnost = errno;
	if (__failed(res)) {
		perror(error_text);
		printf("(%d)\n", errnost);
		exit(1);
	}
}

boolean __failed(int res) {
	return res == -1;
}

void __fail(string error_text) {
	printf("%s\n", error_text);
	exit(1);
}