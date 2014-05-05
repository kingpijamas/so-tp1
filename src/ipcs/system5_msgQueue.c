#include "../../include/system5_msgqueue.h"
#define TMP_FOLDER "/tmp"
#include "../../include/communicator.h"
#include "../../include/common.h"


typedef struct {
	long fromId;
	char data[DATA_SIZE];
} MsgQueuePackage;

#define MSG_SIZE (DATA_SIZE - sizeof(long int))
int ipc_get(int id);

int ipc_connect(int from_id, int to_id){
	return OK;	
}

int ipc_disconnect(int from_id, int to_id){
	return OK;
}

MsgQueuePackage *newMsgQueuePackage(int id, char* data);

void
fatal(char *s)
{
	perror(s);
	exit(1);
}

int ipc_init(int from_id) {
    // ipc_close(from_id);
	return 0;
}

int ipc_get(int key) {
	key_t ipcId;
	ipcId = ftok(TMP_FOLDER, key);
	if (ipcId == (key_t)-1) {
		fatal("Error writing msg - ftok");
		return -1;
	}
	int id = msgget(ipcId, IPC_CREAT | 0666);
	if (id < 0) {
		fatal("Error writing msg - msgget");
		return -1;
	}
	return id;
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	int toIpcId = ipc_get(to_id);
	MsgQueuePackage *msg = newMsgQueuePackage(from_id, buf);
	return msgsnd(toIpcId, (void*) msg, MSG_SIZE,IPC_NOWAIT);
}

int ipc_recv(int from_id, void * buf, int len) {
	int myIpcId = ipc_get(from_id);
	MsgQueuePackage msg;
	//ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
	int result = msgrcv(myIpcId, (void *)&msg, MSG_SIZE, from_id,0);
	if (result == -1) {
		return -1;
	}
	memcpy(buf, msg.data, DATA_SIZE);
	return result;
}

int ipc_close(int from_id) {
	int ipcId = ipc_get(from_id);
	return msgctl(ipcId, IPC_RMID, (struct msqid_ds *) NULL);
}

MsgQueuePackage *newMsgQueuePackage(int id, char* data) {
	MsgQueuePackage* msg = malloc(sizeof(MsgQueuePackage));
	msg->fromId = id;
	memcpy(msg->data, data, DATA_SIZE);
	return msg;
}