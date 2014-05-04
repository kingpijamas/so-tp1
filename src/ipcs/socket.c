#include "../../include/communicator.h"
#include "../../include/utils.h"
#include "../../include/common.h"
#include "../../include/error.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define INVALID -1

#define SRV_ID 0
#define LOCALHOST "127.0.0.1"
#define SRV_ADDRESS LOCALHOST
#define SRV_PORT "1337"
#define BACKLOG 20 //max parallel connections

static void __set_srv_socket(int from_id);
static boolean __failed(int res);
static int __get_conn_socket(int from_id);
static void __verify(int res, string err_text);

static struct addrinfo hints, *res;
static struct sockaddr_storage their_addr;
static socklen_t addr_size;

static int __srv_socket = INVALID, __conn_socket = INVALID;

int ipc_init(int from_id) {
	printf("%s: init...\n", from_id == SRV_ID? "Server":"Client");
	switch(from_id) {
	case SRV_ID:
		ipc_close(from_id);
		__set_srv_socket(from_id);
		__verify(bind(__srv_socket, res->ai_addr, res->ai_addrlen), "Bind error");
		__verify(listen(__srv_socket, BACKLOG), "Listen error");
		return OK;
	default:
		return OK;
	}
}

int ipc_connect(int from_id, int to_id) {
	printf("%s: connecting...\n", from_id == SRV_ID? "Server":"Client");
	switch(from_id) {
	case SRV_ID:
		return OK;
	default:
		__set_srv_socket(from_id);
		__verify(connect(__srv_socket, res->ai_addr, res->ai_addrlen), "Connection error");
		return OK;
	}
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	int written, send_resp, conn_socket = __get_conn_socket(from_id);

	printf("%s: writing to %d...\n", from_id == SRV_ID? "Server":"Client", conn_socket);
	for (written = 0; written < len; written += send_resp) {
		__verify(send_resp = send(conn_socket, buf+written, len-written, 0), "Send error");
		if (send_resp == 0) {
			return written;
		}
	}
	return len;
}

int ipc_recv(int from_id, void * buf, int len) {
	int read, read_resp, conn_socket;

	if (from_id == SRV_ID && __get_conn_socket(from_id) == INVALID) {
		addr_size = sizeof(their_addr);
		__verify(__conn_socket = accept(__srv_socket, (struct sockaddr *)&their_addr, &addr_size), "Accept error");	
	}

	conn_socket = __get_conn_socket(from_id);
	printf("%s: receiving from %d...\n", from_id == SRV_ID? "Server":"Client", conn_socket);
	for (read = 0; read < len; read+=read_resp) {
		//printf("read=%d\n", read);
		__verify(read_resp = recv(conn_socket, buf+read, len-read, 0), "Read error");
		if (read_resp == 0) {
			return read;
		}
	}
	return len;
}

int ipc_disconnect(int from_id, int to_id) {
	switch(from_id) {
		case SRV_ID:
			printf("Server: disconnecting...\n");
			__conn_socket = INVALID;
			return OK;
		default:
			printf("Client: disconnecting...\n");
			close(__get_conn_socket(from_id));
			printf("Client: disconnected\n");
			return OK;
	}
}

int ipc_close(int from_id) {
	printf("%s: closing...\n", from_id == SRV_ID? "Server":"Client");
	if (from_id == SRV_ID) {
		close(__srv_socket);
		if (res != NULL) {
			freeaddrinfo(res);
		}
	}
	return OK;
}

void __set_srv_socket(int from_id) {
	//printf("%s: __set_srv_socket...\n", from_id == SRV_ID? "Server":"Client");
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	__verify(getaddrinfo(SRV_ADDRESS, SRV_PORT, &hints, &res), "Socket creation error");
	__verify(__srv_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol), "Socket creation error");
}

boolean __failed(int res) {
	return res == -1;
}

void __verify(int res, string err_text) {
	verify(!__failed(res), err_text);
}

int __get_conn_socket(int from_id) {
	return (from_id == SRV_ID)? __conn_socket : __srv_socket; 
}