#include "../../include/communicator.h"
#include <string.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/common.h"
#include "../../include/error.h"
#include "../../include/semaphore.h"
#include "../../include/rdwrn.h"
#include "../../include/key.h"

#define SHM_SIZE 100

#define SRV_ID 0

#define SHM_SEM_NUM 3


static void __get_shm();
static void __wipe_shm();
static void __print_sem(int sem_id);
static void __print_all_sem();
static void __print_shm();

static int shm_id, to_read = -1, read = 0;
static char * shm;

typedef enum {
	SEM_READ,
	SEM_WRITE,
	SEM_CONN
} semaphore;

int ipc_init(int from_id) {
	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): init\n", from_id);
		__get_shm();
		__wipe_shm();
		semaphore_init(SHM_SEM_NUM, true);
		__print_all_sem();
		return OK;
	default:
		printf("\nCLT(%d): init\n", from_id);
		return OK; //fail maybe?
	}
}

int ipc_connect(int from_id, int to_id) { // should fail when there is no server
	switch (from_id) {
	case SRV_ID:
		printf("\nSRV(%d): connect (SRV(%d)<->CLT(%d))\n", from_id, from_id, to_id);
		semaphore_let(SEM_WRITE);
		semaphore_let(SEM_CONN);
		return OK; // it's like an accept
	default:
		printf("\nCLT(%d): connect (CLT(%d)<->SRV(%d))\n", from_id, from_id, to_id);
		semaphore_init(SHM_SEM_NUM, false);
		semaphore_stop(SEM_CONN);
		__get_shm();
		return OK;
	}
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): (TRY) send (SRV->CLT(%d)) \"%.*s\", (%d bytes)\n", len, from_id, to_id, (string)buf, len);
		break;
	default:
		printf("\nCLT(%d): (TRY) send (CLT->SRV(%d)) \"%.*s\", (%d bytes)\n", len, from_id, to_id, (string)buf, len);
		break;
	}

	__print_sem(SEM_WRITE);
	semaphore_stop(SEM_WRITE);

	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): send (SRV->CLT(%d)) \"%.*s\", (%d bytes)\n", len, from_id, to_id, (string)buf, len);
		break;
	default:
		printf("\nCLT(%d): send (CLT->SRV(%d)) \"%.*s\", (%d bytes)\n", len, from_id, to_id, (string)buf, len);
		break;
	}

	//printf("SHM before:\n");
	//__print_shm();
	memcpy(shm, &len, sizeof(int));
	memcpy(shm+sizeof(int), buf, len);
	printf("Done writing\n");
	__print_shm();
	
	semaphore_let(SEM_READ);
	__print_sem(SEM_READ);
	//if (from_id != SRV_ID) { //this doesn't either
	//	semaphore_stop(SEM_READ); //double blocking doesn't work
	//}
	return len;
}

int ipc_recv(int from_id, void * buf, int len) {
	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): (TRY) recv (SRV<-CLT) (%d bytes)\n", from_id, len);
		break;
	default:
		printf("\nCLT(%d): (TRY) recv (CLT<-SRV) (%d bytes)\n", from_id, len);
		break;
	}
	if (to_read == -1) {
		__print_sem(SEM_READ);
		semaphore_stop(SEM_READ);
	}

	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): recv (SRV<-CLT) (%d bytes)\n", from_id, len);
		break;
	default:
		printf("\nCLT(%d): recv (CLT<-SRV) (%d bytes)\n", from_id, len);
		break;
	}
	__print_shm();
	printf("(before) to_read: %d, len: %d\n", to_read, len);
	if (to_read == -1) {
		memcpy(&to_read, shm, sizeof(int));
		read = 0;
	}
	printf("(after) to_read: %d, len: %d\n", to_read, len);
	if (len > to_read) {
		memcpy(buf, shm+sizeof(int)+read, to_read); // TODO: circular buffer!
		read += to_read;
		to_read = len-to_read;
	} else {
		memcpy(buf, shm+sizeof(int)+read, len); // TODO: circular buffer!
		read += len;
		if (len == to_read) {
			to_read = -1;
		} else {
			to_read -= len;
		}
	}
	printf("(finally) to_read: %d, len: %d\n", to_read, len);
	if (len == sizeof(char)) {
		printf("Done reading (\"%c\")\n", ((char *)buf)[0]);
	} else if (len == sizeof(int)) {
		printf("Done reading (\"%d\")\n", ((int *) buf)[0]);
	} else {
		printf("Done reading (\"%.*s\")\n", len-1, (string)buf);
	}
	
	if (to_read == -1) {
		semaphore_let(SEM_WRITE);
		__print_sem(SEM_WRITE);
	}
	return OK;
}

int ipc_disconnect(int from_id, int to_id) {
	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): disconnect\n", from_id);
		return OK;
	default:
		printf("\nCLT(%d): disconnect\n", from_id);
		__wipe_shm();
		semaphore_let(SEM_CONN);
		return OK; //fail maybe?
	}
}

int ipc_close(int from_id) {
	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): close\n", from_id);
		semaphore_destroy(SEM_READ);
		semaphore_destroy(SEM_WRITE);
		semaphore_destroy(SEM_CONN);
		shmdt(shm);
		shmctl(shm_id, IPC_RMID, 0);
		return OK;
	default:
		printf("\nCLT(%d): close\n", from_id);
		return OK; //fail maybe?
	}
}

void __wipe_shm() {
	memset(shm, '\0', SHM_SIZE);
}

void __print_all_sem() {
	__print_sem(SEM_CONN);
	__print_sem(SEM_READ);
	__print_sem(SEM_WRITE);
}

void __print_sem(int sem_id) {
	char * name;
	switch(sem_id) {
	case SEM_READ:
		name = "READ";
		break;
	case SEM_WRITE:
		name = "WRITE";
		break;
	case SEM_CONN:
		name = "CONN";
		break;
	/*default:
		printf("\n\n%d is not a valid semaphore id\n\n", sem_id);
		exit(1);*/
	}
	printf("Sem: %s - ", name);
	semaphore_show(sem_id);
}

void __get_shm() {
	assert((shm_id = shmget(key_get('A'), SHM_SIZE, IPC_CREAT /*| IPC_EXCL*/ | 0644)) != -1, "Could not create shared memory area");
	assert((shm = (char*)shmat(shm_id, NULL, 0)) != (void *)-1, "Could not attach shared memory area");
}

void __print_shm() {
	int i;
	printf("memory:\n");
	printf("%d|", ((int *)shm)[0]);
	printf("%d|", ((int *)shm)[1]);
	for(i=2*sizeof(int); i<SHM_SIZE; i++){
		printf("%c|", ((string)shm)[i]);
	}
	printf("\n");
}