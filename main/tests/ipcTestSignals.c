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

// #define SRV_ID 0
// #define CLT_ID 1
#define INVALID -1

#define OK_MSG "OK"
#define NOT_OK_MSG "NK"
#define SRV_RESP_LEN 2

#define SYNC_SEM -1

#define DB_ROOT_PATH "src"
#define SERVER_DIR "server"
#define CLIENT_DIR "client"
#define SERVER_PATH	DB_ROOT_PATH"/"SERVER_DIR
#define CLIENT_PATH	DB_ROOT_PATH"/"CLIENT_DIR

static void __fatal(char *s);
// static void __bye(int sig);
static FILE * __open(char * name, const string mode, const char * path);
static string __get_path(char * name, const char * path);
static char buf[100];

int main(int argc, char **argv) {
	int i = 0;
	char buf[200];
	char * srv="server";
	char * clt="client";
	int srvid;
	int clientid;
	boolean failed = false;
	string messages[] = {"hello", "world!", NULL};
	// ipc_close(CLT_ID); //debería ser disconnect para msgqueue
	// ipc_close(SRV_ID); //debería ser disconnect
	//semaphore_init(SYNC_SEM, true);

	switch (fork()) {
		case -1:
			__fatal("Fork error");
			break;
		case 0: /* child */
			usleep(1000);
			while ( messages[i]!=NULL && !failed ) {
				FILE * file=__open(srv,"r", SERVER_PATH);
				if(file==NULL){
					printf("%s\n","Error opening server id file in client");
				}
				while(fscanf(file,"%d\n", &srvid) != EOF) {;}
				
				clientid=getpid();
				FILE * file2=__open(clt,"w+r",CLIENT_PATH);
				if(file2==NULL){
					printf("%s\n","Error opening client id file");
				}
				fprintf(file2, "%d\n", clientid);
				fclose(file2);

				printf("\nEntro hijo\n");
				// printf("Client id %d, Srv id %d\n",clientid,srvid);
				ipc_connect(clientid, srvid);
				printf("\nChild: about to send (\"%s\")\n", messages[i]);
				ipc_send(clientid, srvid, messages[i], strlen(messages[i]));
				printf("Child: msg sent\n");
				ipc_rcv(clientid, buf, SRV_RESP_LEN);
				printf("Child: response received (%.*s)\n", SRV_RESP_LEN, buf);
				failed = !strneq(OK_MSG, buf, SRV_RESP_LEN);
				if (failed) {
					printf("Child: Error\n");
				  } else {
				  	printf("Child: ok response received\n");
				  }
				ipc_disconnect(clientid, srvid);
				i++;
			}
			//semaphore_let(SYNC_SEM);
			ipc_close(clientid);
			printf("Child: out %s\n",failed? "[ERROR]":"[OK]");
			break;
		default: /* parent */
			printf("\nPadre:\n");
			srvid=getpid();
			FILE * file=__open(srv,"w+r",SERVER_PATH);
			if(file==NULL){
				printf("%s\n","Error opening server id file");
			}
			fprintf(file, "%d\n", srvid);
			fclose(file);
			//
			ipc_init(srvid);
			printf("\nEntro padre\n");
			while ( messages[i]!=NULL ) {
				printf("Parent: about to read\n");
				ipc_rcv(srvid, buf, strlen(messages[i]));
				// 
				FILE * file2=__open(clt,"r",CLIENT_PATH);
				if(file2==NULL){
					printf("%s\n","Error opening client id file2 in parent");
				}
				while(fscanf(file2,"%d\n", &clientid) != EOF) {;}
				//
				printf("Parent: read (\"%.*s\") --(expecting: \"%s\")\n", (int)strlen(messages[i]), buf, messages[i]);
				if (strneq(messages[i], buf, strlen(messages[i]))) {
					printf("Parent: [OK]\n");
					ipc_send(srvid, clientid, OK_MSG, SRV_RESP_LEN);
				} else {
					printf("Parent: [ERROR]\n");
					ipc_send(srvid, clientid, NOT_OK_MSG, SRV_RESP_LEN);
					//exit(1);
				}
				i++;
			}
			ipc_close(srvid);
			printf("Parent: out\n");
			break;
	}
	return 0;
}

void __fatal(char *s) {
	perror(s);
	exit(1);
}

// void __bye(int sig) {
// 	printf("Parent received SIGPIPE\n");
// 	exit(1);
// }

FILE * __open(char * name, const string mode, const char * path) {
	return fopen(__get_path(name,path), mode);
}

string __get_path(char * name, const char * path) {
	sprintf(buf, "%s/%s", path, name); //this should clear the buffer (verify!)
	return buf;
}