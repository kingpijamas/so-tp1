#include "../../include/communicator.h"
#include <string.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/common.h"
#include "../../include/error.h"
#include "../../include/semaphore.h"
#include "../../include/rdwrn.h"

#define SHM_KEY ((key_t) 0x111222)
#define SSHM_KEY ((key_t) 0x110E22)

#define SHM_SIZE 500
#define SSHM_SIZE (sizeof(int))

#define SRV_ID 0

static void __wipe_shm();
static void __attach_shm();

static int shm_id, sshm_id, to_read = -1;
static char * shm, * sshm;

typedef enum {
	READ,
	WRITE,
	CONN
} semaphore;

int ipc_init(int from_id) {
	switch(from_id) {
	case SRV_ID:
		assert((shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT /*| IPC_EXCL*/ | 0666)) != -1, "Could not create shared memory area");
		assert((sshm_id = shmget(SSHM_KEY, SSHM_SIZE, IPC_CREAT /*| IPC_EXCL*/ | 0666)) != -1, "Could not create shared memory area");
		__attach_shm();
		__wipe_shm();
		semaphore_create(READ);
		semaphore_create(WRITE);
		semaphore_create(CONN);
		return OK;
	default:
		return OK; //fail maybe?
	}
}

int ipc_connect(int from_id, int to_id) { // should fail when there is no server
	printf("connection: %d->%d\n", from_id, to_id);
	switch (from_id) {
	case SRV_ID:
		semaphore_let(CONN);
		return OK; // it's like a listen
	default:
		semaphore_stop(CONN);
		__attach_shm();
		return OK;
	}
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	semaphore_stop(READ);
	//semaphore_stop(WRITE);

	memcpy(sshm, &len, sizeof(int));
	memcpy(shm, buf, len);

	//semaphore_let(WRITE);
	semaphore_let(READ);
	return len;
}

int ipc_recv(int from_id, void * buf, int len) {
	semaphore_stop(WRITE);
	//semaphore_stop(READ);

	if (to_read == -1) {
		memcpy(&to_read, sshm, SSHM_SIZE);
	}
	if (len > to_read) {
		readn(shm_id, buf, to_read);
		to_read = len-to_read;
	} else {
		readn(shm_id, buf, len);
		to_read -= len;
	}

	//semaphore_let(READ);
	semaphore_let(WRITE);
	return !OK;
}

int ipc_disconnect(int from_id, int to_id) {
	switch(from_id) {
	case SRV_ID:
		__wipe_shm();
		semaphore_let(CONN);
		return OK;
	default:
		return OK; //fail maybe?
	}
}

int ipc_close(int from_id) {
	switch(from_id) {
	case SRV_ID:
		semaphore_destroy(READ);
		semaphore_destroy(WRITE);
		semaphore_destroy(CONN);
		shmdt(shm);
		shmctl(shm_id, IPC_RMID, 0);
		return OK;
	default:
		return OK; //fail maybe?
	}
}

void __wipe_shm() {
	memset(shm, '\0', SHM_SIZE);
}

void __attach_shm() {
	assert((shm = (char*)shmat(shm_id, NULL, 0)) != (void *)-1, "Could not attach to shared memory area");
	assert((sshm = (char*)shmat(shm_id, NULL, 0)) != (void *)-1, "Could not attach to shared memory area");
}