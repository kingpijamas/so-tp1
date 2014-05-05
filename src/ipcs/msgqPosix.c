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

#define MSQ_MSG_DATA_SIZE (sizeof(__msq_pkg)-sizeof(long))
#define MSQ_MSG_LEN 200


int ipc_init(int from_id) {
    ipc_close(from_id);
    

	return OK;
}

int ipc_connect(int from_id, int to_id){
	
	return OK;
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	
	return len;
}

int ipc_recv(int from_id, void * buf, int len) { // no es lectura buffereada =S
	

	return len;
}

int ipc_disconnect(int from_id, int to_id){
 
	return OK;
}

int ipc_close(int from_id) {
    
	return OK;
}
