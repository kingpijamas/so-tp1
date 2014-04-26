#include "posix_messagequeues.h"
// TODO: Change names to use it for server AND client. Think about it. 
// TODO: And then test, obviously.

MsgQueuePackage * newMsgQueuePackage(int id, char * data);

int ipc_init(int fromid){	
}

int ipc_send(int fromid, int toId, void * buf, int len){
	
	mqd_t mqdOut;
	char *servname = "/server_queue";
	int srvFlg=O_CREATE|O_WRONLY;
	mode_t mode=0666;
	struct mq_attr attr;
	attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = len;
	struct msgQueuePackage * messagePtr= newMsgQueuePackage(getId(),(char *)buf); 
	int offset=(*messagePtr).msg-messagePtr;

	if((mqdOut=mq_open(servname, srvFlg, mode, &mq_attr))==ERROR){
		fatal("Error in message queue descriptor for server")
	}

	if(mq_send(mqdOut,messagePtr,len+offset, MSG_PRIORITY)==ERROR){
		fatal("Error sending message to server");
	}

	return OK;
}


int ipc_recv(void * buf, int len, int toId){
	mqd_t mqdIn;
	char cltname[CLTNAMESIZE];
	int cltFlg=O_CREATE|O_RDONLY;
	mode_t mode=0666;	
	
	struct mq_attr attr;

	attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = len;
	
	sprintf(cltname, "/%ld_queue", toId);
	if((mqdIn=mq_open(&cltname, cltFlg, mode, &mq_attr))==ERROR){
		fatal("Error opening msgq in queue descriptor for client");
	}
	if((mq_receive(mqdIn,buf,len,NULL))==ERROR){
		fatal();
	}

}

int ipc_close(int fromid){
	char cltname[CLTNAMESIZE];
	sprintf(cltname, "/client_%d_queue", fromid);
	mq_unlink(cltname);
	printf("Message queue closed\n");
}

MsgQueuePackage * newMsgQueuePackage(int id, char * data){
	MsgQueuePackage * msgPackptr=malloc(sizeof(MsgQueuePackage));
	(*msgPackptr).fromId=id;
	memcpy((*msgPackptr).msg,data,MSG_SIZE);
	return msgPackptr;
}