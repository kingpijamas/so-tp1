#include "../../include/communicator.h"
#include "../../include/utils.h"
#include "../../include/common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define SRV_ID 0
#define SRV_IP "127.0.0.1"
#define SRV_PORT "3490"
#define BACKLOG 20 //max parallel connections

static void __set_socket();
static void __connect();
static void __accept();
static void __assert(int res, string error_text);
static boolean __failed(int res);
static int __recv(void * buf, int len);
static int __send(void * buf, int len);

static struct addrinfo hints, *res;
static struct sockaddr_storage their_addr;
static socklen_t addr_size;

static int * srv_socket = NULL;
static int * conn_socket = NULL;

int ipc_init(int from_id) {
	//	case SRV_ID:
	__set_socket();
	__assert(bind(*srv_socket, res->ai_addr, res->ai_addrlen), "Bind error");
	__assert(listen(*srv_socket, BACKLOG), "Listen error");
	return OK;
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	switch(from_id) {
	case SRV_ID:
		if (conn_socket == NULL) {
			return !OK;
		}
		break;
	default:
		//pruebo hacer send... si falla, me conecto y hago send
		if (!__failed(__send(buf, len))) {
			return OK;
		}
		__connect();
		break;
	}
	__assert(__send(buf, len), "Send error");
	return OK;
}

int ipc_recv(int from_id, void * buf, int len) {
	switch(from_id) {
	case SRV_ID:
		if (!__failed(__recv(buf, len))) {
			return OK;
		}
		__accept();
		break;
	default:
		if (conn_socket == NULL) {
			return !OK;
		}
		break;
	}
	__assert(__recv(buf, len), "Read error");
	return OK;
}

int ipc_close(int from_id) {
	if (from_id == SRV_ID) {
		close(*srv_socket);
		//TODO: open connection case
	}
	freeaddrinfo(res);
	return OK;
}

void __set_socket() {
	if (srv_socket!=NULL) {
		return;
	}
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(SRV_IP, SRV_PORT, &hints, &res);
	*srv_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	__assert(*srv_socket, "Socket creation error");
}

int __recv(void * buf, int len) {
	return recv(*conn_socket, buf, len, 0);
}

int __send(void * buf, int len) {
	return send(*conn_socket, buf, len, 0);
}

void __connect() {
	__set_socket();
	__assert(connect(*srv_socket, res->ai_addr, res->ai_addrlen), "Connection error");
}

void __accept() {
	__set_socket();
	addr_size = sizeof(their_addr);
	*conn_socket = accept(*srv_socket, (struct sockaddr *)&their_addr, &addr_size);
	__assert(*conn_socket, "Accept error");
}

void __assert(int res, string error_text) {
	if (__failed(res)) {
		perror(error_text);
		exit(1);
	}
}

boolean __failed(int res) {
	return res == -1;
}