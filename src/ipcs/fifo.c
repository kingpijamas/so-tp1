#include "../../include/communicator.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "../../include/utils.h"
#include "../../include/rdwrn.h"
#include "../../include/common.h"

#define FIFO_DIR "/tmp/so-fifo"
#define FIFO_BUFFER_SIZE 100

#define ALL_RW S_IRWXU|S_IRWXG|S_IRWXO

static fdesc __get_fifo(string ipc_name, int flags);

static char __buf[FIFO_BUFFER_SIZE];

int ipc_init(int from_id) {
	//since it's a concurrent server, from_id and to_id are ignored here
	return mkdir(FIFO_DIR, ALL_RW);
}

int ipc_connect(int from_id, int to_id) {
	//semaforo y fue (si no es uwait que es un asco, o andar creando y matando los fifos srv->clt y clt->srv?)
	return OK;
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	//bloquearse si ya hay un ipc funcionando
	//printf("Writing %d bytes to %s from id: %d\n", len, ipc_name, from_id);
	printf("%s: Writing %d bytes to %s\n", from_id==0? "Srv":"Clt", len, ipc_name);
	return /*writen(*/write(__get_fifo(ipc_name, O_WRONLY), buf, len);
}

int ipc_recv(int from_id, void * buf, int len) {
	//bloquearse si ya hay un ipc funcionando
	//printf("Reading %d bytes from %s from id: %d\n", len, ipc_name, from_id);
	printf("%s: Reading %d bytes from %s\n", from_id==0? "Srv":"Clt", len, ipc_name);
	return /*readn(*/read(__get_fifo(from_id, SRV_ID, O_RDONLY), buf, len);
}

int ipc_disconnect(int from_id, int to_id) {
	return OK;
}

int ipc_close(int from_id) {
	unlink(FIFO_CLT_SRV_FULL_NAME);
	unlink(FIFO_SRV_CLT_FULL_NAME);
	rmdir(FIFO_DIR);
	return OK;
}

fdesc __get_fifo(int from_id, int to_id, int flags) {
	sprintf(__buf, FIFO_DIR"/(%d->%d)", from_id, to_id);
	mkfifo(__buf, ALL_RW | S_IFIFO);
	return open(__buf, flags | S_IFIFO);
}