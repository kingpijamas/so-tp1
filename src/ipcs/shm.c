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
#include "../../include/utils.h"

#define SHM_SIZE 100
#define SHM_MSG_LEN (SHM_SIZE - sizeof(int))

#define SHM_SEM_NUM 3

static void __get_shm();
static void __wipe_shm();
static void __print_sem(int sem_id);
static void __print_all_sem();
static void __print_shm();

static int __shm_id, __to_read = -1, __read = 0;
static char * __shm;

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
		semaphore_let(SEM_CONN);
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
		semaphore_let(SEM_CLT);
		__print_sem(SEM_CLT);
		return OK; // it's like an accept
	default:
		printf("\nCLT(%d): connect (CLT(%d)<->SRV(%d))\n", from_id, from_id, to_id);
		semaphore_init(SHM_SEM_NUM, false);
		semaphore_stop(SEM_CONN);
		__get_shm();
		__print_all_sem();
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
		printf("\nSRV(%d): (TRY) send (SRV->CLT(%d)) \"%.*s\", (%d bytes)\n", from_id, to_id, len, (string)buf, len);
		break;
	default:
		printf("\nCLT(%d): (TRY) send (CLT->SRV(%d)) \"%.*s\", (%d bytes)\n", from_id, to_id, len, (string)buf, len);
		__print_sem(SEM_CLT);
		semaphore_stop(SEM_CLT);
		break;
	}

	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): send (SRV->CLT(%d)) \"%.*s\", (%d bytes)\n", from_id, to_id, len, (string)buf, len);
		break;
	default:
		printf("\nCLT(%d): send (CLT->SRV(%d)) \"%.*s\", (%d bytes)\n", from_id, to_id, len, (string)buf, len);
		break;
	}

	memcpy(__shm, &len, sizeof(int));
	memcpy(__shm+sizeof(int), buf, min(len, SHM_MSG_LEN));
	printf("Done writing\n");
	__print_shm();
	
	switch (from_id) {
	case SRV_ID:
		if(semaphore_get_val(SEM_CLT) == 0) {
			semaphore_let(SEM_CLT);
			__print_sem(SEM_CLT);
		}else{
			printf("[CACA detected]\n\n");
			exit(1);
		}
		break;
	default:
		semaphore_let(SEM_SRV);
		__print_sem(SEM_SRV);
		break;
	}
	return min(len, SHM_MSG_LEN);
}

//CLT		SRV
// 1(send) 
//       	1(recv)
//		 	1(send)
// 1(recv)
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
	__print_all_sem();
	if (__to_read == -1) { // done reading
		switch(from_id) {
		case SRV_ID:
			//__print_sem(SEM_SRV);
			semaphore_stop(SEM_SRV);
			break;
		default:
			//__print_sem(SEM_CLT);
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
	printf("(before) __to_read: %d, len: %d\n", __to_read, len);
	if (__to_read == -1) {
		memcpy(&__to_read, __shm, sizeof(int));
		__read = 0;
	}

	printf("__read: %d, len: %d, SHM_MSG_LEN: %d (%s)\n", __read, len, SHM_MSG_LEN, (__read + len < SHM_MSG_LEN)? "true":"false");
	if (__read + len < SHM_MSG_LEN) {
		memcpy(buf, __shm+sizeof(int)+__read, len);
	} else {
		memcpy(buf, __shm+sizeof(int)+(__read%SHM_MSG_LEN), SHM_MSG_LEN - (__read % SHM_MSG_LEN));
		memcpy(buf, __shm+sizeof(int), (__read+len-SHM_MSG_LEN) % SHM_MSG_LEN);
	}
	__read += len;

	if (__read == __to_read) {
		__to_read = -1;
		__read = 0;
	}
	return len;
}

int ipc_disconnect(int from_id, int to_id) {
	switch(from_id) {
	case SRV_ID:
		printf("\nSRV(%d): disconnect\n", from_id);
		return OK;
	default:
		printf("\nCLT(%d): disconnect\n", from_id);
		__wipe_shm();
		__print_all_sem();
//TESING
		if (semaphore_get_val(SEM_CONN) == 0) {
			semaphore_let(SEM_CONN);
		} else {
			printf("[CACA detected]\n\n");
			exit(1);
		}
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
		shmdt(__shm);
		shmctl(__shm_id, IPC_RMID, 0);
		return OK;
	default:
		printf("\nCLT(%d): close\n", from_id);
		return OK; //fail maybe?
	}
}

void __wipe_shm() {
	memset(__shm, '\0', SHM_SIZE);
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
	printf("Sem val: %d\n", semaphore_get_val(sem_id));
}

void __get_shm() {
	verify((__shm_id = shmget(key_get('A'), SHM_SIZE, IPC_CREAT /*| IPC_EXCL*/ | 0644)) != -1, "Could not create shared memory area");
	verify((__shm = (char*)shmat(__shm_id, NULL, 0)) != (void *)-1, "Could not attach shared memory area");
}

void __print_shm() {
	int i;
	printf("memory:\n");
	printf("%d|", ((int *)__shm)[0]);
	for(i=sizeof(int); i<SHM_SIZE; i++){
		printf("%c|", ((string)__shm)[i]);
	}
	printf("\n");
}