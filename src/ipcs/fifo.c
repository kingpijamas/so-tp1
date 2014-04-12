#include "../../include/communicator.h"

int ipc_init(int fromid) {
	return -1;
}

//fromid toId from the processes
int ipc_send(int fromid, int toId, void * buf, int len) {
	return -1;
}

//who called recv could easily go into the message (if necessary)
int ipc_recv(void * buf, int len) {
	return -1;
}

int ipc_close(int fromid) {
	return -1;
}
