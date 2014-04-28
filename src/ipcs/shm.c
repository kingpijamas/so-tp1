#include "../../include/communicator.h"
#include <string.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/common.h"

#define MEM_KEY ((key_t) 0x111222)
#define SHM_SIZE 2000

static void __assert(int res, string error_text);
static boolean __failed(int res);
static void __fail(string error_text);

static int shm_id;
static char * shm;

int ipc_init(int from_id) {
	__assert(shm_id = shmget(MEM_KEY, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666), "Could not allocate memory");
	shm = (char*)shmat(shm_id, NULL, 0);
	if (shm == (void *)-1) {
		__fail("Could not allocate memory");
	}
	memset(shm, '\0', SHM_SIZE);
	return OK;
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	memcpy(shm, buf, len);
	return len;
}

int ipc_recv(int from_id, void * buf, int len) {
	memcpy(buf, shm, len);
	return len;
}

int ipc_close(int from_id) {
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