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

#define INVALID -1

#define OK_MSG "OK"
#define NOT_OK_MSG "NK"
#define SRV_RESP_LEN 2

#define SYNC_SEM -1

#define ROOT_PATH "src"
#define SERVER_DIR "server"
#define CLIENT_DIR "client"
#define SERVER_PATH	ROOT_PATH"/"SERVER_DIR
#define CLIENT_PATH	ROOT_PATH"/"CLIENT_DIR

static void __fatal(char *s);
static FILE * __open(char * name, const string mode, const char * path);
static string __get_path(char * name, const char * path);

static char buf[100];

int main(int argc, char **argv) {
	int i = 0, client_id;
	char buf[200];
	boolean failed = false;
	FILE * file;
	string messages[] = {"hello", "world!", NULL};

	switch (fork()) {
		case -1:
			__fatal("Fork error");
			break;
		case 0: /* child */
			usleep(1000);
			client_id=getpid();
			while ( messages[i]!=NULL && !failed ) {
				printf("\nEntro hijo\n");
				ipc_connect(client_id, SRV_ID);
				printf("\nChild: about to send (\"%s\")\n", messages[i]);
				ipc_send(client_id, SRV_ID, messages[i], strlen(messages[i]));
				printf("Child: msg sent\n");
				ipc_recv(client_id, buf, SRV_RESP_LEN);
				printf("Child: response received (%.*s)\n", SRV_RESP_LEN, buf);
				failed = !strneq(OK_MSG, buf, SRV_RESP_LEN);
				if (failed) {
					printf("Child: Error\n");
				} else {
				  	printf("Child: ok response received\n");
				}
				ipc_disconnect(client_id, SRV_ID);
				i++;
			}
			ipc_close(client_id);
			printf("Child: out %s\n",failed? "[ERROR]":"[OK]");
			break;
		default: /* parent */
			printf("\nPadre:\n");			
			printf("Server id in Test%d\n", getpid());
			ipc_init(SRV_ID);
			printf("\nEntro padre\n");
			while ( messages[i]!=NULL ) {
				printf("Parent: about to read\n");
				ipc_recv(SRV_ID, buf, strlen(messages[i]));
				// I can't omit this because usually the server receives the id at the beginning
				file=__open("client", "r", CLIENT_PATH);
				if(file==NULL){
					printf("%s\n","Error opening client id file in parent");
				}
				while(fscanf(file,"%d\n", &client_id) != EOF) {;}
				printf("Client id %d\n",client_id);
				printf("Parent: read (\"%.*s\") --(expecting: \"%s\")\n", (int)strlen(messages[i]), buf, messages[i]);
				if (strneq(messages[i], buf, strlen(messages[i]))) {
					printf("Parent: [OK]\n");
					ipc_send(SRV_ID, client_id, OK_MSG, SRV_RESP_LEN);
				} else {
					printf("Parent: [ERROR]\n");
					ipc_send(SRV_ID, client_id, NOT_OK_MSG, SRV_RESP_LEN);
				}
				file = NULL;
				i++;
			}
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

FILE * __open(char * name, const string mode, const char * path) {
	return fopen(__get_path(name,path), mode);
}

string __get_path(char * name, const char * path) {
	sprintf(buf, "%s/%s", path, name); //this should clear the buffer (verify!)
	return buf;
}