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

#define SHM_SEM_NUM 3

static void __get_shm();
static void __wipe_shm();
static void __print_sem(int sem_id);
static void __print_all_sem();
static void __print_shm();

static int shm_id, to_read = -1, read = 0;
static char * shm;

typedef enum {
	SEM_SRV,
	SEM_CLT,
	SEM_CONN
} __semaphore;

int ipc_init(int from_id) {
	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): init\n", from_id);
		__get_shm();
		__wipe_shm();
		__print_shm();
		semaphore_init(SHM_SEM_NUM, true);
		__print_all_sem();
		semaphore_let(SEM_CONN);
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
		semaphore_let(SEM_CLT);
		return OK; // it's like an accept
	default:
		printf("\nCLT(%d): connect (CLT(%d)<->SRV(%d))\n", from_id, from_id, to_id);
		semaphore_init(SHM_SEM_NUM, false);
		semaphore_stop(SEM_CONN);
		__get_shm();
		return OK;
	}
}

//CLT		SRV
// 1(send) 
//       	1(recv)
//		 	1(send)
// 1(recv)

int ipc_send(int from_id, int to_id, void * buf, int len) {
	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): (TRY) send (SRV->CLT(%d)) \"%.*s\", (%d bytes)\n", len, from_id, to_id, (string)buf, len);
		break;
	default:
		printf("\nCLT(%d): (TRY) send (CLT->SRV(%d)) \"%.*s\", (%d bytes)\n", len, from_id, to_id, (string)buf, len);
		__print_sem(SEM_CLT);
		semaphore_stop(SEM_CLT);
		break;
	}

	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): send (SRV->CLT(%d)) \"%.*s\", (%d bytes)\n", len, from_id, to_id, (string)buf, len);
		break;
	default:
		printf("\nCLT(%d): send (CLT->SRV(%d)) \"%.*s\", (%d bytes)\n", len, from_id, to_id, (string)buf, len);
		break;
	}

	memcpy(shm, &len, sizeof(int));
	memcpy(shm+sizeof(int), buf, len);
	printf("Done writing\n");
	__print_shm();
	
	switch (from_id) {
	case SRV_ID:
		semaphore_let(SEM_CLT);
		__print_sem(SEM_CLT);
		break;
	default:
		semaphore_let(SEM_SRV);
		__print_sem(SEM_SRV);
		break;
	}
	return len;
}

//CLT		SRV
// 1(send) 
//       	1(recv)
//		 	1(send)
// 1(recv)

int ipc_recv(int from_id, void * buf, int len) {
	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): (TRY) recv (SRV<-CLT) (%d bytes)\n", from_id, len);
		break;
	default:
		printf("\nCLT(%d): (TRY) recv (CLT<-SRV) (%d bytes)\n", from_id, len);
		break;
	}
	if (to_read == -1) { // done reading
		switch(from_id) {
		case SRV_ID:
			__print_sem(SEM_SRV);
			semaphore_stop(SEM_SRV);
			break;
		default:
			__print_sem(SEM_CLT);
			semaphore_stop(SEM_CLT);
			break;
		}
	}
	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): recv (SRV<-CLT) (%d bytes)\n", from_id, len);
		break;
	default:
		printf("\nCLT(%d): recv (CLT<-SRV) (%d bytes)\n", from_id, len);
		break;
	}
//TODO: buffered reading!
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
		semaphore_destroy(SEM_SRV);
		semaphore_destroy(SEM_CLT);
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
	__print_sem(SEM_SRV);
	__print_sem(SEM_CLT);
}

void __print_sem(int sem_id) {
	char * name;
	switch(sem_id) {
	case SEM_SRV:
		name = "SRV";
		break;
	case SEM_CLT:
		name = "CLT";
		break;
	case SEM_CONN:
		name = "CONN";
		break;
	/*default:
		printf("\n\n%d is not a valid __semaphore id\n\n", sem_id);
		exit(1);*/
	}
	printf("Sem: %s - ", name);
	semaphore_show(sem_id);
}

void __get_shm() {
	verify((shm_id = shmget(key_get('A'), SHM_SIZE, IPC_CREAT /*| IPC_EXCL*/ | 0644)) != -1, "Could not create shared memory area");
	verify((shm = (char*)shmat(shm_id, NULL, 0)) != (void *)-1, "Could not attach shared memory area");
}

void __print_shm() {
	int i;
	printf("memory:\n");
	printf("%d|", ((int *)shm)[0]);
	for(i=sizeof(int); i<SHM_SIZE; i++){
		printf("%c|", ((string)shm)[i]);
	}
	printf("\n");
}