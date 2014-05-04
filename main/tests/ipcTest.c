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
// #include <fcntl.h>           /* For O_* constants */
// #include <sys/stat.h>        /* For mode constants */
// #include <mqueue.h>

#define SRV_ID 0
#define CLT_ID 1
#define INVALID -1

#define OK_MSG "OK"
#define NOT_OK_MSG "NK"
#define SRV_RESP_LEN 2

#define SYNC_SEM -1


static void __fatal(char *s);
static void __bye(int sig);

int main(int argc, char **argv) {
	int i = 0;
	char buf[200];
	boolean failed = false;
	string messages[] = {"hello", "world!", NULL};
	ipc_close(CLT_ID);
	ipc_close(SRV_ID);
	//semaphore_init(SYNC_SEM, true);

	switch (fork()) {
		case -1:
			__fatal("Fork error");
			break;
		case 0: /* child */
			usleep(1000);
			//printf("\nHijo:\n");
			//semaphore_stop(SYNC_SEM);
			while ( messages[i]!=NULL && !failed ) {
				printf("\nEntro hijo\n");
				ipc_connect(CLT_ID, SRV_ID);
				printf("\nChild: about to send (\"%s\")\n", messages[i]);
				ipc_send(CLT_ID, 0, messages[i], strlen(messages[i]));
				printf("Child: msg sent\n");
				ipc_rcv(CLT_ID, buf, SRV_RESP_LEN);
				printf("Child: response received (%.*s)\n", SRV_RESP_LEN, buf);
				failed = !strneq(OK_MSG, buf, SRV_RESP_LEN);
				if (failed) {
					printf("Child: Error\n");
				  } else {
				  	printf("Child: ok response received\n");
				  }
				ipc_disconnect(CLT_ID, SRV_ID);
				i++;
			}
			//semaphore_let(SYNC_SEM);
			ipc_close(CLT_ID);
			printf("Child: out %s\n",failed? "[ERROR]":"[OK]");
			break;
		default: /* parent */
			printf("\nPadre:\n");
			// signal(SIGPIPE, __bye);
			ipc_init(SRV_ID);
			ipc_connect(SRV_ID, INVALID);
			// //semaphore_let(SYNC_SEM);
			printf("\nEntro padre\n");

			while ( messages[i]!=NULL ) {
				printf("Parent: about to read\n");
				ipc_rcv(SRV_ID, buf, strlen(messages[i]));
				printf("Parent: read (\"%.*s\") --(expecting: \"%s\")\n", (int)strlen(messages[i]), buf, messages[i]);
				if (strneq(messages[i], buf, strlen(messages[i]))) {
					printf("Parent: [OK]\n");
					ipc_send(SRV_ID, CLT_ID, OK_MSG, SRV_RESP_LEN);
				} else {
					printf("Parent: [ERROR]\n");
					ipc_send(SRV_ID, CLT_ID, NOT_OK_MSG, SRV_RESP_LEN);
					//exit(1);
				}
				i++;
			}
			// // usleep(1000);
			// //semaphore_stop(SYNC_SEM);
			ipc_close(SRV_ID);
			printf("Parent: out\n");
			break;
	}
	return 0;
}

void __fatal(char *s) {
	perror(s);
	exit(1);
}

void __bye(int sig) {
	printf("Parent received SIGPIPE\n");
	exit(1);
}