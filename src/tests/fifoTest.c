#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../include/common.h"
#include "../../include/utils.h"
#include "../ipcs/fifo.h"

#define INVALID -1
#define OK_MSG "OK"
#define NOT_OK_MSG "NK"

void fatal(char *s) {
	perror(s);
	exit(1);
}

void bye(int sig) {
	printf("Parent received SIGPIPE\n");
	exit(1);
}

int main(int argc, char **argv) {
	int i = 0;
	char buf[200];
	string messages[] = {"hello", "world!", NULL};

	ipc_close(INVALID);
	switch ( fork() ) {
		case -1:
			fatal("Fork error");
			break;
		case 0: /* child */
			ipc_init(INVALID);
			while ( messages[i]!=NULL ) {
				printf("Child: msg sent\n");
				ipc_send(INVALID, INVALID, messages[i], strlen(messages[i]));
				ipc_recv(buf, strlen(OK_MSG));
				printf("Child: response received\n");
				if ( !streq(OK_MSG, buf) ) {
					ipc_close(INVALID);
					fatal("Error - child");
				} else {
					printf("Child: ok response received\n");
				}
				i++;
			}
			ipc_close(INVALID);
			printf("Child out\n");
			break;
		default: /* parent */
			signal(SIGPIPE, bye);
			while ( messages[i]!=NULL ) {
				ipc_recv(buf, strlen(messages[i]));
				if ( streq(messages[i], buf) ) {
					printf("[OK]\n");
					ipc_send(INVALID, INVALID, OK_MSG, strlen(OK_MSG));
				} else {
					printf("[ERROR]\n");
					ipc_send(INVALID, INVALID, NOT_OK_MSG, strlen(NOT_OK_MSG));
				}
				i++;
			}
			printf("Parent out\n");
			break;
	}
	return 0;
}
