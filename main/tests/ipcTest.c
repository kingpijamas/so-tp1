#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../include/common.h"
#include "../../include/utils.h"
#include "../../include/communicator.h"
#include "../../include/semaphore.h"

#define SRV_ID 0
#define CLT_ID 1
#define INVALID -1

#define OK_MSG "OK"
#define NOT_OK_MSG "NK"
#define SRV_RESP_LEN 2

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
	boolean failed = false;
	string messages[] = {"hello", "world!", NULL};

	ipc_close(SRV_ID);

	switch (fork()) {
		case -1:
			fatal("Fork error");
			break;
		case 0: /* child */
			while ( messages[i]!=NULL && !failed ) {
				ipc_connect(CLT_ID, SRV_ID);
				printf("Child: about to send (\"%s\")\n", messages[i]);
				ipc_send(CLT_ID, 0, messages[i], strlen(messages[i]));
				printf("Child: msg sent\n");
				ipc_recv(CLT_ID, buf, SRV_RESP_LEN);
				printf("Child: response received (%.*s)\n", SRV_RESP_LEN, buf);
				failed = !strneq(OK_MSG, buf, SRV_RESP_LEN);
				if (failed) {
					printf("Child: Error\n");
				} else {
					printf("Child: ok response received\n");
				}
				i++;
			}
			ipc_close(INVALID);
			printf("Child: out %s\n",failed? "[ERROR]":"[OK]");
			break;
		default: /* parent */
			signal(SIGPIPE, bye);
			ipc_init(SRV_ID);
			ipc_connect(SRV_ID, INVALID);
			while ( messages[i]!=NULL ) {
				printf("Parent: about to read\n");
				ipc_recv(SRV_ID, buf, strlen(messages[i]));
				printf("Parent: read (\"%.*s\") --(expecting: \"%s\")\n", strlen(messages[i]), buf, messages[i]);
				if (strneq(messages[i], buf, strlen(messages[i]))) {
					printf("Parent: [OK]\n");
					ipc_send(SRV_ID, CLT_ID, OK_MSG, SRV_RESP_LEN);
				} else {
					printf("Parent: [ERROR]\n");
					ipc_send(SRV_ID, CLT_ID, NOT_OK_MSG, SRV_RESP_LEN);
				}
				i++;
			}
			ipc_disconnect(SRV_ID, CLT_ID);
			printf("Parent: out\n");
			break;
	}
	return 0;
}