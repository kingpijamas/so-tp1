#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "fifo.h"
#include "../../include/utils.h"
#include "../../include/common.h"

#define FIFO_IPC_DIR "/tmp/so-fifo"
#define FIFO_IPC_NAME "fifo"
#define FIFO_IPC_FULL_NAME FIFO_IPC_DIR"/"FIFO_IPC_NAME

#define ALL_PERMS S_IRWXU|S_IRWXG|S_IRWXO

static fdesc __connect(int flags);

int ipc_init(int from_id) {
	//since it's a concurrent server, from_id and to_id are ignored here
	return mkdir(FIFO_IPC_DIR, ALL_PERMS);
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	//since it's a concurrent server, from_id and to_id are ignored here
	return write(__connect(O_WRONLY), buf, len);
}

int ipc_recv(void * buf, int len) {
	return read(__connect(O_RDONLY), buf, len);
}

int ipc_close(int from_id) {
	unlink(FIFO_IPC_FULL_NAME);
	unlink(FIFO_IPC_DIR);
	return OK;
}

fdesc __connect(int flags) {
	mkfifo(FIFO_IPC_FULL_NAME, ALL_PERMS | S_IFIFO);
	return open(FIFO_IPC_FULL_NAME, flags | S_IFIFO);
}