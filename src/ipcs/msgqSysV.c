#include "../../include/communicator.h"
#include "../../include/common.h"
#include "../../include/key.h"
#include "../../include/error.h"
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define DATA_SIZE 100
#define MSG_SIZE (DATA_SIZE - sizeof(long int))

typedef struct {
	long fromId;
	char data[DATA_SIZE];
} __msgq_package;

static int ipc_get(int id);
static __msgq_package * __msgq_package_new(int id, char* data);

int ipc_init(int from_id) {
    ipc_close(from_id);
	return OK;
}

int ipc_connect(int from_id, int to_id){
	return OK;
}

int ipc_get(int key) {
	key_t ipcId;
	int id;
	
	verify((ipcId = key_get('Q')) != (key_t)-1, "Error writing msg - ftok");
	verify((id = msgget(ipcId, IPC_CREAT | 0666)) >= 0, "Error writing msg - msgget");
	return id;
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	int toIpcId = ipc_get(to_id);
	__msgq_package *msg = __msgq_package_new(from_id, buf);
	return msgsnd(toIpcId, (void*) msg, MSG_SIZE, IPC_NOWAIT);
}

int ipc_recv(int from_id, void * buf, int len) {
	int myIpcId = ipc_get(from_id);
	__msgq_package msg;
	//ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
	int result = msgrcv(myIpcId, (void *)&msg, MSG_SIZE, from_id,0);
	if (result == -1) {
		return -1;
	}
	memcpy(buf, msg.data, DATA_SIZE);
	return result;
}

int ipc_disconnect(int from_id, int to_id){
	int ipcId = ipc_get(from_id);
	return msgctl(ipcId, IPC_RMID, (struct msqid_ds *) NULL);
}

int ipc_close(int from_id) {
	return OK;	
}

__msgq_package *__msgq_package_new(int id, char * data) {
	__msgq_package * msg = malloc(sizeof(__msgq_package));
	msg->fromId = id;
	memcpy(msg->data, data, DATA_SIZE);
	return msg;
}