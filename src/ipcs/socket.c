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
#define SRV_PORT "99999"
#define BACKLOG 20 //max parallel connections

static void __set_socket(int from_id);
static boolean __failed(int res);

static struct addrinfo hints, *res;
static struct sockaddr_storage their_addr;
static socklen_t addr_size;

static int srv_socket = INVALID;
static int conn_socket = INVALID;

int ipc_init(int from_id) {
	switch(from_id) {
	case SRV_ID:
		__set_socket(from_id);
		assert(!__failed(bind(srv_socket, res->ai_addr, res->ai_addrlen)), "Bind error");
		assert(!__failed(listen(srv_socket, BACKLOG)), "Listen error");
		return OK;
	default:
		return OK;
	}
}

int ipc_connect(int from_id, int to_id) {
	switch(from_id) {
	case SRV_ID:
		break;
	default:
		__set_socket(from_id);
		assert(!__failed(connect(srv_socket, res->ai_addr, res->ai_addrlen)), "Connection error");
		break;
	}
}

int ipc_send(int from_id, int to_id, void * buf, int len) {
	assert(!__failed(send(conn_socket, buf, len, 0)), "Send error");
	return OK;
}

int ipc_recv(int from_id, void * buf, int len) {
	switch(from_id) {
	case SRV_ID:
		__set_socket(from_id);
		addr_size = sizeof(their_addr);
		conn_socket = accept(srv_socket, (struct sockaddr *)&their_addr, &addr_size);
		assert(!__failed(conn_socket), "Accept error");
		break;
	default:
		break;
	}
	assert(!__failed(recv(conn_socket, buf, len, 0)), "Read error");
	return OK;
}

int ipc_disconnect(int from_id, int to_id) {
	close(conn_socket);
	freeaddrinfo(res);
	return OK;
}

int ipc_close(int from_id) {
	if (from_id == SRV_ID) {
		close(srv_socket);
	}
	freeaddrinfo(res);
	return OK;
}

void __set_socket(int from_id) {
	char * address;
	if (srv_socket != INVALID) {
		return;
	}
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_STREAM;
	if (from_id == SRV_ID) {
		printf("\nServer: connecting...\n");
		hints.ai_flags = AI_PASSIVE; //use my IP
		address = NULL;
	} else {
		printf("\nClient: connecting...\n");
		address = LOCALHOST;
	}
	assert(!__failed(getaddrinfo(address, SRV_PORT, &hints, &res)), "Socket creation error");
	assert(!__failed(srv_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)), "Socket creation error");
}

boolean __failed(int res) {
	return res == -1;
}