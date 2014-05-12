#include "../../include/communicator.h"
#include "../../include/common.h"
#include "../../include/key.h"
#include "../../include/error.h"
#include "../../include/utils.h"
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define MQ_BASE_NAME "so-mq-posix"

#define MQ_MSG_DATA_SIZE (MQ_MSG_LEN+sizeof(int))
#define MQ_MSG_LEN 200

static int __destr_mq(int from_id);
static mqd_t __get_mqd(int owner_id, boolean creat);
static mqd_t __get_mqd_strict(int owner_id, boolean creat);
static string __as_mq_name(int owner_id);

static int __to_read = -1, __read = 0;
static char __msg_data_buf[MQ_MSG_DATA_SIZE];


int ipc_init(int from_id) {
	__destr_mq(from_id);

    printf("%s: Initializing...\n", (from_id == SRV_ID)? "Srv":"Clt");
	switch (from_id) {
	case SRV_ID:
		__get_mqd_strict(from_id, true);
		return OK;
	default:
		return OK;
	}
}

int ipc_connect(int from_id, int to_id){
	printf("%s: Connecting...\n", (from_id == SRV_ID)? "Srv":"Clt");
	__to_read = -1;
	__read = 0;
	switch (from_id) {
	case SRV_ID:
		return OK;
	default:
		__get_mqd_strict(from_id, true);
		return OK;
	}
}

int ipc_send(int from_id, int to_id, void * buf, int len) { // no buffered writing
	int sent = min(len, MQ_MSG_LEN);
	
	printf("\n(Try) %s: Sending %d bytes to %d through %s...\n", (from_id == SRV_ID)? "Srv":"Clt", len, to_id, __as_mq_name(to_id));

	memcpy(__msg_data_buf, &sent, sizeof(int));
	memcpy(__msg_data_buf+sizeof(int), buf, sent);
	verify(mq_send(__get_mqd_strict(to_id, false), __msg_data_buf, MQ_MSG_DATA_SIZE, 0) != -1, "Send error");

	printf("%s: Sent %d bytes through %s...\n", (from_id == SRV_ID)? "Srv":"Clt", sent, __as_mq_name(to_id));
	return sent;
}

int ipc_recv(int from_id, void * buf, int len) {
	 printf("\n(Try) %s: Receiving %d bytes through %s...\n", (from_id == SRV_ID)? "Srv":"Clt", len, __as_mq_name(from_id));
	
	if (__to_read == -1) {
		verify(mq_receive(__get_mqd_strict(from_id, false), __msg_data_buf, MQ_MSG_DATA_SIZE, NULL) != -1, "Receive error");
		__to_read = ((int *)__msg_data_buf)[0];
		__read = 0;
	}

	if (__read + len < MQ_MSG_LEN) {
		memcpy(buf, __msg_data_buf+sizeof(int)+__read, len);
	} else {
		memcpy(buf, __msg_data_buf+sizeof(int)+(__read%MQ_MSG_LEN), MQ_MSG_LEN - (__read % MQ_MSG_LEN));
		memcpy(buf, __msg_data_buf+sizeof(int), (__read+len-MQ_MSG_LEN) % MQ_MSG_LEN);
	}
	__read += len;

	if (__read == __to_read) {
		__to_read = -1;
		__read = 0;
		printf("%s: Received %d bytes through %s... \n", (from_id == SRV_ID)? "Srv":"Clt", len, __as_mq_name(from_id));
	} else {
	    printf("%s: Received %d bytes through %s... \n", (from_id == SRV_ID)? "Srv":"Clt", len, __as_mq_name(from_id));
	}
	return len;
}

int ipc_disconnect(int from_id, int to_id){
	printf("%s: Disconnecting... from %s\n", (from_id == SRV_ID)? "Srv":"Clt", __as_mq_name(from_id));
	switch (from_id) {
	case SRV_ID:
		return OK;
	default:
		verify(__destr_mq(from_id) != -1, "Error closing ipc");
		return OK;
	}
}

int ipc_close(int from_id) {
    printf("%s: Closing...\n", (from_id == SRV_ID)? "Srv":"Clt");
	switch (from_id) {
	case SRV_ID:
		verify(__destr_mq(from_id) != -1, "Error closing ipc");
		return OK;
	default:
		return OK;
	}
}

int __destr_mq(int from_id) {
	mqd_t mqd;
	string mq_name;
	
	if ((mqd = __get_mqd(from_id, false)) == (mqd_t)-1) {
		return -1;
	}

	mq_close(mqd);
	mq_name = __as_mq_name(from_id);
	return mq_unlink(mq_name);
}

mqd_t __get_mqd_strict(int owner_id, boolean creat) {
	mqd_t mqd = __get_mqd(owner_id, creat);
	verify(mqd != (mqd_t) -1, creat? "Error creating message queue":"Error getting message queue");
	return mqd;
}

mqd_t __get_mqd(int owner_id, boolean creat) {
	string mq_name = __as_mq_name(owner_id);
	int base_oflags = O_RDWR;
	int perms = 0666;
	struct mq_attr attr;
	
	if (creat) {
		attr.mq_flags = 0;
		attr.mq_maxmsg = 1;
		attr.mq_msgsize = MQ_MSG_DATA_SIZE;
		attr.mq_curmsgs = 0; ///?
		return mq_open(mq_name, base_oflags | O_CREAT, perms, &attr);
	}
	return mq_open(mq_name, base_oflags);
}

string __as_mq_name(int owner_id) {
	void * buf = malloc(200);
	if (buf == NULL) {
		fail("Could not allocate memory");
	}
	sprintf(buf, "/"MQ_BASE_NAME"-%d", owner_id);
	return (string) buf;
}
