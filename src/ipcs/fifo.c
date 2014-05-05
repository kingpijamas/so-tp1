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
#include "../../include/error.h"

#define FIFO_BUFFER_SIZE 100
#define FIFO_DIR "/tmp/so-fifo"

#define ALL_RW S_IRWXU|S_IRWXG|S_IRWXO
#define FIFO_NAME(from,to) "("from"->"to")"
#define FIFO_SRV_CLT_FULL_NAME FIFO_DIR"/"FIFO_NAME("srv","clt")
#define FIFO_CLT_SRV_FULL_NAME FIFO_DIR"/"FIFO_NAME("clt","srv")

static fdesc __get_fifo(int receiver_id, int flags);
static int __mk_fifo(string fifo_name);
static string __get_fifo_name(int receiver_id);
static boolean __failed(int res);
static void __verify(int res, string err_text);

int ipc_init(int from_id) {
	switch(from_id) {
	case SRV_ID:
		ipc_disconnect(from_id, -1);
		return mkdir(FIFO_DIR, ALL_RW);
	default:
		return OK;
	}
}

int ipc_connect(int from_id, int to_id) {
	struct stat fifo_stat;
	int errnost;

	printf("%s: Connecting...\n", from_id==SRV_ID? "Srv":"Clt");
	switch(from_id) {
	case SRV_ID:
		while(stat(FIFO_SRV_CLT_FULL_NAME, &fifo_stat) == -1) {
			errnost = errno;
			verify(errnost == EBADF || errnost == ENOENT, "IPC: Error connecting (SRV)");
			usleep(100);
		}
		return OK;
	default:
		while(stat(FIFO_SRV_CLT_FULL_NAME, &fifo_stat) == 0) {
			usleep(100);
		}
		errnost = errno;
		verify(errnost == EBADF || errnost == ENOENT, "IPC: Error connecting (CLT)");
		__verify(__mk_fifo(FIFO_SRV_CLT_FULL_NAME), "IPC: Error connecting (CLT)");
		__verify(__mk_fifo(FIFO_CLT_SRV_FULL_NAME), "IPC: Error connecting (CLT)");
		return OK;
	}
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	int fifo_fd, written, resp;
	printf("(Try) %s: Writing %d bytes to %s\n", from_id==SRV_ID? "Srv":"Clt", len, __get_fifo_name(to_id));

	fifo_fd = __get_fifo(to_id, O_WRONLY);
	for (written = 0; written < len; written += resp) {
		__verify(resp = fwriten(fifo_fd, buf+written, len-written), "IPC: Send error");
		if (resp == 0) {
			return written;
		}
	}
	printf("%s: Wrote %d bytes to %s\n", from_id==SRV_ID? "Srv":"Clt", len, __get_fifo_name(to_id));
	return len;
}

int ipc_recv(int from_id, void * buf, int len) {
	int fifo_fd, read, resp;
	printf("(Try) %s: Reading %d bytes from %s\n", from_id==SRV_ID? "Srv":"Clt", len, __get_fifo_name(from_id));

	fifo_fd = __get_fifo(from_id, O_RDONLY);
	for (read = 0; read < len; read += resp) {
		__verify(resp = freadn(fifo_fd, buf+read, len-read), "IPC: Recv error");
		if (resp == 0) {
			return read;
		}
	}
	printf("%s: Read %d bytes from %s\n", from_id==SRV_ID? "Srv":"Clt", len, __get_fifo_name(from_id));
	return len;
}

int ipc_disconnect(int from_id, int to_id) {
	printf("%s: Disconnecting...\n", from_id==SRV_ID? "Srv":"Clt");
	switch (from_id) {
	case SRV_ID:
		return OK;
	default:
		unlink(FIFO_CLT_SRV_FULL_NAME);
		unlink(FIFO_SRV_CLT_FULL_NAME);
		return OK;
	}
}

int ipc_close(int from_id) {
	switch(from_id) {
	case SRV_ID:
		rmdir(FIFO_DIR);
		return OK;
	default:
		return OK;
	}
}

fdesc __get_fifo(int receiver_id, int flags) {
	return open(__get_fifo_name(receiver_id), flags /*| S_IFIFO*/);
}

int __mk_fifo(string fifo_name) {
	return mkfifo(fifo_name, ALL_RW | S_IFIFO);
}

string __get_fifo_name(int receiver_id) {
	switch(receiver_id) {
	case SRV_ID:
		return FIFO_CLT_SRV_FULL_NAME;
	default:
		return FIFO_SRV_CLT_FULL_NAME;
	}
}

boolean __failed(int res) {
	return res == -1;
}

void __verify(int res, string err_text) {
	verify(!__failed(res), err_text);
}