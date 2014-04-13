#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "fifo.h"
#include "../../include/utils.h"
#include "../../include/common.h"

#define FIFO_IPC_NAME(from,to) "("from"->"to")"

#define FIFO_IPC_DIR "/tmp/so-fifo"
#define FIFO_IPC_SRV_CLT_FULL_NAME FIFO_IPC_DIR"/"FIFO_IPC_NAME("srv","clt")
#define FIFO_IPC_CLT_SRV_FULL_NAME FIFO_IPC_DIR"/"FIFO_IPC_NAME("clt","srv")

#define ALL_RW S_IRWXU|S_IRWXG|S_IRWXO

static string __get_ipc_name(int from_id);
static fdesc __connect(string ipc_name, int flags);


int ipc_init(int from_id) {
	//since it's a concurrent server, from_id and to_id are ignored here
	return mkdir(FIFO_IPC_DIR, ALL_RW);
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	//since it's a concurrent server, from_id and to_id are ignored here
	string ipc_name = __get_ipc_name(from_id);
	// printf("Writing to %s from id: %d\n", ipc_name, from_id);
	return write(__connect(ipc_name, O_WRONLY), buf, len);
}

int ipc_recv(int from_id, void * buf, int len) {
	string ipc_name = __get_ipc_name(from_id);
	// printf("Reading from %s from id: %d\n", ipc_name, from_id);
	return read(__connect(ipc_name, O_RDONLY), buf, len);
}

int ipc_close(int from_id) {
	unlink(FIFO_IPC_CLT_SRV_FULL_NAME);
	unlink(FIFO_IPC_SRV_CLT_FULL_NAME);
	return OK;
}

string __get_ipc_name(int from_id) {
	string ipc_name;
	switch(from_id) {
		case 0: //I'm the server
			ipc_name = FIFO_IPC_SRV_CLT_FULL_NAME;
		default:
			ipc_name = FIFO_IPC_CLT_SRV_FULL_NAME;
	}
	return ipc_name;
}

fdesc __connect(string ipc_name, int flags) {
	mkfifo(ipc_name, ALL_RW | S_IFIFO);
	return open(ipc_name, flags | S_IFIFO);
}