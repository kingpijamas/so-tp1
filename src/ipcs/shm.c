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

#define SHM_SIZE 500
#define SHM_KEY ((key_t) 0x111222)

#define SRV_ID 0

#define SHM_SEM_NUM 3


static void __get_shm();
static void __wipe_shm();
static void __print_sem(int sem_id);
static void __print_all_sem();

static int shm_id, to_read = -1;
static char * shm;

typedef enum {
	SEM_READ,
	SEM_WRITE,
	SEM_CONN
} semaphore;

int ipc_init(int from_id) {
	switch(from_id) {
	case SRV_ID:
		printf("SRV(%d): init\n", from_id);
		__get_shm();
		__wipe_shm();
		semaphore_init(SHM_SEM_NUM, true);
		__print_all_sem();
		return OK;
	default:
		printf("CLT(%d): init\n", from_id);
		return OK; //fail maybe?
	}
}

int ipc_connect(int from_id, int to_id) { // should fail when there is no server
	switch (from_id) {
	case SRV_ID:
		printf("SRV(%d): connect (SRV(%d)<->CLT(%d))\n", from_id, from_id, to_id);
		semaphore_let(SEM_WRITE);
		semaphore_let(SEM_CONN);
		return OK; // it's like an accept
	default:
		printf("CLT(%d): connect (CLT(%d)<->SRV(%d))\n", from_id, from_id, to_id);
		semaphore_init(SHM_SEM_NUM, false);
		semaphore_stop(SEM_CONN);
		__get_shm();
		return OK;
	}
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	switch(from_id) {
	case SRV_ID:
		printf("SRV(%d): (TRY) send (SRV->CLT(%d)) \"%s\", (%d bytes)\n", from_id, to_id, (string)buf, len);
		break;
	default:
		printf("CLT(%d): (TRY) send (CLT->SRV(%d)) \"%s\", (%d bytes)\n", from_id, to_id, (string)buf, len);
		break;
	}

	__print_sem(SEM_WRITE);
	semaphore_stop(SEM_WRITE);

	switch(from_id) {
	case SRV_ID:
		printf("SRV(%d): send (SRV->CLT(%d)) \"%s\", (%d bytes)\n", from_id, to_id, (string)buf, len);
		break;
	default:
		printf("CLT(%d): send (CLT->SRV(%d)) \"%s\", (%d bytes)\n", from_id, to_id, (string)buf, len);
		break;
	}
	
	memcpy(shm, &len, sizeof(int));
	memcpy(shm+sizeof(int), buf, len);
	printf("Done writing\n");
	
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
		printf("SRV(%d): (TRY) recv (SRV<-CLT) (%d bytes)\n", from_id, len);
		break;
	default:
		printf("CLT(%d): (TRY) recv (CLT<-SRV) (%d bytes)\n", from_id, len);
		break;
	}
	//if (to_read == -1) { NO se puede hacer asi, porque el primer recv del srv se queda tildado
		// NO deberia bloquearse si to_read != -1 (todavia no termine de leer)
		__print_sem(SEM_READ);
		semaphore_stop(SEM_READ);
	//}
	switch(from_id) {
	case SRV_ID:
		printf("SRV(%d): recv (SRV<-CLT) (%d bytes)\n", from_id, len);
		break;
	default:
		printf("CLT(%d): recv (CLT<-SRV) (%d bytes)\n", from_id, len);
		break;
	}

	if (to_read == -1) {
		memcpy(&to_read, shm, sizeof(int));
	}
	if (len > to_read) {
		memcpy(buf, shm+sizeof(int), to_read);
		to_read = len-to_read;
	} else {
		memcpy(buf, shm+sizeof(int), len);
		if (len == to_read) {
			to_read = -1;
		} else {
			to_read -= len;
		}
	}
	printf("Done reading\n");	
	
//	if (to_read != -1) {

//	} 
	semaphore_let(SEM_WRITE);
	__print_sem(SEM_WRITE);
//	if(to_read!=-1) {
//	semaphore_stop(SEM_WRITE);
//	}
//	semaphore_stop(SEM_WRITE); double blocking doesn't work
	return OK;
}

int ipc_disconnect(int from_id, int to_id) {
	switch(from_id) {
	case SRV_ID:
		printf("SRV(%d): disconnect\n", from_id);
		return OK;
	default:
		printf("CLT(%d): disconnect\n", from_id);
		__wipe_shm();
		semaphore_let(SEM_CONN);
		return OK; //fail maybe?
	}
}

int ipc_close(int from_id) {
	switch(from_id) {
	case SRV_ID:
		printf("SRV(%d): close\n", from_id);
		semaphore_destroy(SEM_READ);
		semaphore_destroy(SEM_WRITE);
		semaphore_destroy(SEM_CONN);
		shmdt(shm);
		shmctl(shm_id, IPC_RMID, 0);
		return OK;
	default:
		printf("CLT(%d): close\n", from_id);
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