#include "../../include/communicator.h"
#include "../../include/common.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#define PDATA_SIZE 10

typedef struct {
	long fromId;
	char data[PDATA_SIZE];
} MsgQueuePackage;

#define MSG_SIZE (PDATA_SIZE - sizeof(long int))

static struct mq_attr __attr_init();
int ipc_get(int id);
MsgQueuePackage *newMsgQueuePackage(int id, char* data);


int ipc_connect(int from_id, int to_id){
	return OK;	
}

int ipc_disconnect(int from_id, int to_id){
	return OK;
}

void
fatal(char *s)
{
	perror(s);
	exit(1);
}

int ipc_init(int from_id) {
	return OK;
}

int ipc_get(int key) {
	return OK;
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	struct mq_attr attr=__attr_init();
	char ipcname[20];
	sprintf(ipcname, "/%d", to_id);
	mqd_t mqdt=mq_open(ipcname,O_RDWR|O_CREAT, 0666, &attr);
	if (mqdt == -1) {
		fatal("Error opening posix msgqueue with mq_open");
		return ERROR;
	}
	MsgQueuePackage *msg = newMsgQueuePackage(from_id, buf);
	int retsend=mq_send(mqdt, (void*) msg, MSG_SIZE,0);
	if(retsend==-1){
		return retsend;
	}
	int ret=mq_close(mqdt);
	if(ret==-1){
		return ret;
	}
	return retsend;
}


int ipc_recv(int from_id, void * buf, int len) {
	struct mq_attr attr=__attr_init();
	char ipcname[20];
	sprintf(ipcname, "/%d", from_id);
	// printf("IPC NAME %s\n",ipcname);
	mqd_t mqdt=mq_open(ipcname,O_RDWR|O_CREAT,0666,&attr);
	if (mqdt == -1) {
		fatal("Error opening posix msgqueue with mq_open");
		return ERROR;
	}
	MsgQueuePackage msg;
	int result = mq_receive(mqdt, (void *)&msg, MSG_SIZE, 0);
	if (result == -1) {
		printf("%s\n","Error in receive");
		perror(&errno);
		return -1;
	}
	memcpy(buf, msg.data, PDATA_SIZE);
	int ret=mq_close(mqdt);
	if(ret==-1){
		return ret;
	}
	return result;
}

int ipc_close(int from_id) {
	char ipcname[20];
	sprintf(ipcname, "/%d", from_id);
	mqd_t mqdt=mq_open(ipcname, O_RDWR);
	int ret=mq_close(mqdt);
	if(ret==-1){
		return ret;
	}
	return mq_unlink(ipcname);
}

MsgQueuePackage *newMsgQueuePackage(int id, char* data) {
	MsgQueuePackage* msg = malloc(sizeof(MsgQueuePackage));
	msg->fromId = id;
	memcpy(msg->data, data, PDATA_SIZE);
	return msg;
}

struct mq_attr __attr_init(){
	struct mq_attr attr;
	attr.mq_flags = 0;  
	attr.mq_maxmsg = 10;  
	attr.mq_msgsize = MSG_SIZE;  
	attr.mq_curmsgs = 0;
	return attr;
}