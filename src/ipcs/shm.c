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

#define SRV_MSG_FORMAT "\t\t\t\t\t\t\t\t|\t\t\t"

static void __get_shm();
static void __wipe_shm();
static void __print_sem(int from_id, int sem_id);
static void __print_all_sem(int from_id);
static void __print_shm(int from_id);

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
		printf("\n%s< SRV(%d): (TRY) init\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id);
		__get_shm();
		__wipe_shm();
		semaphore_init(SHM_SEM_NUM, true);
		__print_all_sem(from_id);
		semaphore_let(SEM_CONN);
		printf("\n");
		__print_all_sem(from_id);
		printf("%sSRV(%d): init >\n\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id);
		return OK;
	default:
		printf("\n< CLT(%d): (TRY) init\n", from_id);
		printf("CLT(%d): init >\n\n", from_id);
		return OK; //fail maybe?
	}
}

int ipc_connect(int from_id, int to_id) { // should fail when there is no server
	switch (from_id) {
	case SRV_ID:
		printf("\n%s< SRV(%d): (TRY) connect (SRV(%d)<->CLT(%d))\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id, from_id, to_id);
		__print_all_sem(from_id);
		semaphore_let(SEM_CLT);
		printf("\n");
		__print_all_sem(from_id);
		printf("\n%sSRV(%d): connect (SRV(%d)<->CLT(%d)) >\n\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id, from_id, to_id);
		return OK; // it's like an accept
	default:
		printf("\n< CLT(%d): (TRY) connect (CLT(%d)<->SRV(%d))\n", from_id, from_id, to_id);
		semaphore_init(SHM_SEM_NUM, false);
		__print_all_sem(from_id);
		semaphore_stop(SEM_CONN);
		__get_shm();
		printf("\n");
		__print_all_sem(from_id);
		printf("\nCLT(%d): connect (CLT(%d)<->SRV(%d)) >\n\n", from_id, from_id, to_id);
		return OK;
	}
}

//CLT		SRV
// 1(send) 
//       	1(recv)
//		 	1(send)
// 1(recv)

int ipc_send(int from_id, int to_id, void * buf, int len) {
	int sent = min(len, SHM_MSG_LEN);
	switch(from_id) {
	case SRV_ID:
		printf("\n%s< SRV(%d): (TRY) send (SRV->CLT(%d)) \"%.*s\", (%d bytes)\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id, to_id, len, (string)buf, len);
		printf("\n%sSRV(%d): send (SRV->CLT(%d)) \"%.*s\", (%d bytes)\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id, to_id, sent, (string)buf, sent);
		break;
	default:
		printf("\n< CLT(%d): (TRY) send (CLT->SRV(%d)) \"%.*s\", (%d bytes)\n", from_id, to_id, len, (string)buf, len);
		__print_all_sem(from_id);
		semaphore_stop(SEM_CLT);
		printf("\n");
		__print_all_sem(from_id);
		printf("\nCLT(%d): send (CLT->SRV(%d)) \"%.*s\", (%d bytes)\n", from_id, to_id, sent, (string)buf, sent);
		break;
	}

	memcpy(__shm, &sent, sizeof(int));
	memcpy(__shm+sizeof(int), buf, min(len, SHM_MSG_LEN));
	printf("%sDone writing\n", from_id == SRV_ID? SRV_MSG_FORMAT:"");

	__print_all_sem(from_id);
	printf("\n");
	switch (from_id) {
	case SRV_ID:
		semaphore_let(SEM_CLT);
		break;
	default:
		semaphore_let(SEM_SRV);
		break;
	}
	__print_all_sem(from_id);
	printf("\n%s(%d): sent (SRV->CLT(%d)) \"%.*s\", (%d bytes) >\n\n", from_id == SRV_ID? SRV_MSG_FORMAT"SRV":"CLT", from_id, to_id, sent, (string)buf, sent);
	return sent;
}

int ipc_recv(int from_id, void * buf, int len) {
	switch(from_id) {
	case SRV_ID:
		printf("\n%s< SRV(%d): (TRY) recv (SRV<-CLT) (%d bytes)\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id, len);
		break;
	default:
		printf("\n< CLT(%d): (TRY) recv (CLT<-SRV) (%d bytes)\n", from_id, len);
		break;
	}
	__print_all_sem(from_id);
	printf("%s(really before) __to_read: %d\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", __to_read);
	if (__to_read == -1) { // done reading
		switch(from_id) {
		case SRV_ID:
			semaphore_stop(SEM_SRV);
			break;
		default:
			semaphore_stop(SEM_CLT);
			break;
		}
	}
	printf("\n");
	__print_all_sem(from_id);
	switch(from_id) {
	case SRV_ID:
		printf("\n%sSRV(%d): recv (SRV<-CLT) (%d bytes)\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id, len);
		break;
	default:
		printf("\nCLT(%d): recv (CLT<-SRV) (%d bytes)\n", from_id, len);
		break;
	}
	printf("%s(before) __to_read: %d, len: %d\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", __to_read, len);
	if (__to_read == -1) {
		memcpy(&__to_read, __shm, sizeof(int));
		__read = 0;
	}

	printf("%s__read: %d, len: %d, SHM_MSG_LEN: %d (%s)\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", __read, len, SHM_MSG_LEN, (__read + len <  SHM_MSG_LEN)? "true":"false");
	if (__read + len <  SHM_MSG_LEN) {
		memcpy(buf, __shm+sizeof(int)+__read, len);
	} else {
		memcpy(buf, __shm+sizeof(int)+(__read%SHM_MSG_LEN), SHM_MSG_LEN - (__read % SHM_MSG_LEN));
		memcpy(buf, __shm+sizeof(int), (__read+len-SHM_MSG_LEN) % SHM_MSG_LEN);
	}
	if(len != sizeof(int)) {
		printf("%sread word: '%.*s'\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", len, (string) buf);		
	} else {
		printf("%sread int: '%d'\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", ((int *)buf)[0]);		
	}
	__read += len;

	if (__read == __to_read) {
		__to_read = -1;
		__read = 0;
	}

	printf("%s(after) __to_read: %d, __read: %d, len: %d >\n\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", __to_read, __read, len);
	return len;
}

int ipc_disconnect(int from_id, int to_id) {
	switch(from_id) {
	case SRV_ID:
		printf("\n%s< SRV(%d): (TRY) disconnect\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id);
		__print_all_sem(from_id);
		printf("\n%sSRV(%d): disconnect >\n\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id);
		return OK;
	default:
		printf("\n< CLT(%d): (TRY) disconnect\n", from_id);
		__wipe_shm();
		__print_all_sem(from_id);

		if (semaphore_get_val(SEM_CONN) != 0) {
			printf("%s[CACA detected]\n\n", from_id == SRV_ID? SRV_MSG_FORMAT:"");
			exit(1);
		}

		semaphore_let(SEM_CONN);
		printf("\n");
		__print_all_sem(from_id);

		printf("\nCLT(%d): disconnect >\n\n", from_id);
		return OK; //fail maybe?
	}
}

int ipc_close(int from_id) {
	switch(from_id) {
	case SRV_ID:
		printf("\n%s< SRV(%d): (TRY) close\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id);
		semaphore_destroy(SEM_SRV);
		semaphore_destroy(SEM_CLT);
		semaphore_destroy(SEM_CONN);
		shmdt(__shm);
		shmctl(__shm_id, IPC_RMID, 0);
		printf("\n%sSRV(%d): close >\n\n", from_id == SRV_ID? SRV_MSG_FORMAT:"", from_id);
		return OK;
	default:
		printf("\n< CLT(%d): (TRY) close\n", from_id);
		printf("\nCLT(%d): close >\n\n", from_id);
		return OK; //fail maybe?
	}
}

void __get_shm() {
	verify((__shm_id = shmget(key_get('A'), SHM_SIZE, IPC_CREAT | 0644)) != -1, "Could not create shared memory area");
	verify((__shm = (char*)shmat(__shm_id, NULL, 0)) != (void *)-1, "Could not attach shared memory area");
}

void __wipe_shm() {
	memset(__shm, '\0', SHM_SIZE);
}

void __print_all_sem(int from_id) {
	__print_sem(from_id, SEM_CONN);
	__print_sem(from_id, SEM_CLT);
	__print_sem(from_id, SEM_SRV);
}

void __print_sem(int from_id, int sem_id) {
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
	}
	printf("%sSem: %s - ", from_id == SRV_ID? SRV_MSG_FORMAT:"", name);
	printf("Sem val: %d\n", semaphore_get_val(sem_id));
}

void __print_shm(int from_id) {
	int i;
	string prefix = from_id == SRV_ID? SRV_MSG_FORMAT:"";
	printf("%smemory:\n", prefix);
	printf("%s%d|", prefix, ((int *)__shm)[0]);
	for(i=sizeof(int); i< SHM_SIZE; i++){
		printf("%c|", ((string)__shm)[i]);
	}
	printf("\n");
}

