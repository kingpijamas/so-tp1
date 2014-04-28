#include "../../include/communicator.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#define SRV_IP "127.0.0.1"
#define SRV_PORT "3490"
#define BACKLOG 20 //max parallel connections

static void __set_socket();
static void __connect();
static void __accept();
static void __assert(int res, string error_text);
static boolean __failed(int res);

static struct addrinfo hints, *res;
static struct sockaddr_storage their_addr;
static socklen_t addr_size;
static int * srv_socket = NULL;
static int * conn_socket = NULL;

int ipc_init(int from_id) {
	switch(from_id) {
		case SRV_ID:
			__set_socket();
			__assert(bind(*srv_socket, res->ai_addr, res->ai_addrlen), "Bind error");
			__assert(listen(*srv_socket, BACKLOG), "Listen error");
			return OK;
		default:
			return !OK;
	}
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	switch(from_id) {
	case SRV_ID:
		if (conn_socket == NULL) {
			return !OK;
		}
		__assert(send(*conn_socket, socket, buf, len, 0));
		return OK;
	default:
		//pruebo hacer send... si falla, me conecto y hago send
		if (__failed(send(*srv_socket, buf, len, 0)) {
			__connect();
		}
		__assert(send(*srv_socket, buf, len, 0)));
		return OK;
	}
}

int ipc_recv(int from_id, void * buf, int len) {
	switch(from_id) {
	case SRV_ID:
		if (__failed(recv(*conn_socket, buf, len, 0))) {
			__accept();
		}
		__assert(recv(*conn_socket, buf, len, 0)));
		return OK;
	default:
		if (conn_socket == NULL) {
			return !OK;
		}
		__assert(recv(*conn_socket, buf, len, 0));
		return OK;
	}
}

int ipc_close(int from_id) {
	switch (from_id) {
	case SRV_ID:
		close(sockfd);
		freeaddrinfo(servinfo);
		break;
	default:
		freeaddrinfo(servinfo);
		break;
	}
}

void __set_socket() {
	if (srv_socket!=NULL) {
		return OK;
	}
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(SRV_IP, SRV_PORT, &hints, &res);
	*srv_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	__assert(*srv_socket, "Socket creation error"));
}

void __connect() {
	__set_socket();
	__assert(connect(*srv_socket, res->ai_addr, res->ai_addrlen), "Connection error"));
}

void __accept() {
	__set_socket();
	addr_size = sizeof(their_addr);
	*conn_socket = accept(*srv_socket, (struct sockaddr *)&their_addr, &addr_size));
	__assert(*conn_socket, "Accept error");
}

void __assert(int res, string error_text) {
	if (__failed(res)) {
		perror(errno, error_text);
		exit(1);
	}
}

boolean __failed(int res) {
	return res == -1;
}