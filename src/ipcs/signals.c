#include "../../include/communicator.h"
#include "../../include/utils.h"
#include "../../include/rdwrn.h"
#include "../../include/common.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#define _POSIX_SOURCE

#define INVALID -1
#define SIGNALFILES_IPC_DIR "src"
#define BUFFER_SIZE 100
#define UNEXPECTED_DELETE_ERROR 2

#define ALL_RW S_IRWXU|S_IRWXG|S_IRWXO

#define SIGNAL_MSG_DATA_SIZE (SIGNAL_MSG_LEN+sizeof(int))
#define SIGNAL_MSG_LEN 200

#define DB_ROOT_PATH "src"
#define SERVER_DIR "server"
#define CLIENT_DIR "client"
#define SERVER_PATH	DB_ROOT_PATH"/"SERVER_DIR
#define CLIENT_PATH	DB_ROOT_PATH"/"CLIENT_DIR

static void __handler(int n);
static string __get_path(char * name, const char * path);
static int __write_new(char * id) ;
static FILE * __open(char * name, const string mode, const char * path);
static int __id_Server(int id);

static int __exit_flag, __to_read = -1, __read = 0;
static char __msg_data_buf[SIGNAL_MSG_DATA_SIZE];

//Only the server uses init & close.
int ipc_init(int from_id){
	if(from_id==SRV_ID){
		from_id=getpid();
	}
	FILE * file=__open("server", "w+r", SERVER_PATH);
	if(file==NULL){
		printf("%s\n","Error opening server id file");
	}
	fprintf(file, "%d\n", from_id);
	fclose(file);

	ipc_connect(from_id, INVALID);
	return OK;
}

//Create the file from_id if it does not exist
int ipc_connect(int from_id, int to_id){
	char ipcname[20];
	int ret=OK;

	__to_read = -1;
	__read = 0;
	if (from_id == SRV_ID) {
		return OK;
	}

	int srvid=__id_Server(from_id);
	if(srvid!=false){
		from_id=srvid;
	}else{
		srvid=__id_Server(to_id);
		if(srvid!=false){
			to_id=from_id;
		}
	}

	sprintf(ipcname, "%d", from_id);
	//Creates the "from" file to pass the information 
	FILE * file=__open(ipcname,"r",SIGNALFILES_IPC_DIR);
	if(file==NULL){
		ret=__write_new(ipcname);
	}else{
		fclose(file);
	}
	printf("From id %d To id %d\n",from_id,to_id);
	if(to_id!=INVALID){
		printf("%s\n","Client");
		//Client
		int clientid=getpid();
		FILE * file2=__open("client", "w+r", CLIENT_PATH);
		if(file2==NULL){
		printf("%s\n","Error opening client id file");
		}
		fprintf(file2, "%d\n", clientid);
		fclose(file2);
	}else{
		//Server
	}
	return ret;
}

//Write to_id file. Send signal.
int ipc_send(int from_id, int to_id, void * buf, int len){
	
	int srvid=__id_Server(from_id);
	if(srvid!=false){
		from_id=srvid;
	}else{
		srvid=__id_Server(to_id);
		if(srvid!=false){
			to_id=srvid;
		}
	}

	char ipcname[20];
	sprintf(ipcname, "%d", to_id);
	FILE * file=__open(ipcname, "r+w",SIGNALFILES_IPC_DIR);
	if(file == NULL){
		printf("After open, file null. ipcname:%s\n to_id:%d",ipcname,to_id);
		return ERROR;
	}
	int fd = fileno(file);
	int ret = fwriten(fd, &len, sizeof(int));
	if (ret != -1) {
		fwriten(fd, buf, len); //atomic.
	}
	if(ret == -1){
		printf("%s\n","Fwriten returning -1");
		return ERROR;
	}
	printf("%s\n","Sending signal");
	kill(to_id,SIGUSR1);
	return fclose(file);
}

//Reads from_id file
int ipc_recv(int from_id, void * buf, int len){
	sigset_t mask, oldmask;
	char ipcname[20];
	struct sigaction act;

	if (__to_read == -1) {
		// Gets server id
		int srv=__id_Server(from_id);
		if(srv!=false){
			from_id=srv;
		}
		//Change the action taken by a process on receipt of the signal
		memset (&act, '\0', sizeof(act));
		act.sa_handler = &__handler;
		if (sigaction(SIGUSR1, &act, NULL) < 0) {
			perror ("sigaction");
			return 1;
		}
	 	// Set up the mask of signals to temporarily block. 
	    sigemptyset (&mask);
	    sigaddset (&mask, SIGUSR1);
		// Wait for the signal
		sigprocmask (SIG_BLOCK, &mask, &oldmask);
		while(!__exit_flag){
			sigsuspend (&oldmask);
		}
		sigprocmask (SIG_UNBLOCK, &mask, NULL);

		sprintf(ipcname, "%d", from_id);
		FILE * file = __open(ipcname, "r+",SIGNALFILES_IPC_DIR);
		if(file==NULL){
			printf("%s\n","Null"); 
			return ERROR;
		}
		freadn(fileno(file), __msg_data_buf, SIGNAL_MSG_DATA_SIZE);
		__exit_flag=0;

		__to_read = ((int *) __msg_data_buf)[0];
		__read = 0;
	}


	if (__read + len < SIGNAL_MSG_LEN) {
		memcpy(buf, __msg_data_buf+sizeof(int)+__read, len);
	} else {
		memcpy(buf, __msg_data_buf+sizeof(int)+(__read%SIGNAL_MSG_LEN), SIGNAL_MSG_LEN - (__read % SIGNAL_MSG_LEN));
		memcpy(buf, __msg_data_buf+sizeof(int), (__read+len-SIGNAL_MSG_LEN) % SIGNAL_MSG_LEN);
	}
	__read += len;


	if (__read == __to_read) {
		__to_read = -1;
		__read = 0;
	}
	return len;
}

//Delete client file
int ipc_disconnect(int from_id, int to_id){
	char ipcname[20];

	switch(from_id) {
	case SRV_ID:
		return OK;
	default:
		sprintf(ipcname, "%d", from_id);
		if (remove(__get_path(ipcname, SIGNALFILES_IPC_DIR)) != 0){
			return UNEXPECTED_DELETE_ERROR;
		}
		return OK;		
	}
}

//Delete server file.
int ipc_close(int from_id){
	int srvid=__id_Server(from_id);
	from_id=srvid;
	char ipcname[20];
	sprintf(ipcname, "%d", from_id);
	if (remove(__get_path(ipcname,SIGNALFILES_IPC_DIR)) != 0){
		return UNEXPECTED_DELETE_ERROR;
	}
	return OK;
}

int __write_new(char * id) {
	FILE * file = __open(id, "w+",SIGNALFILES_IPC_DIR);
	if (file==NULL){
		printf("%s\n","__write_new file null");
		return ERROR;
	}
	close(fileno(file));
	return OK;
}

FILE * __open(char * name, const string mode, const char * path) {
	return fopen(__get_path(name,path), mode);
}

string __get_path(char * name, const char * path) {
	string buf = malloc(200);
	sprintf(buf, "%s/%s", path, name); //this should clear the buffer (verify!)
	printf("BUF %s\n", buf);
	return buf;
}

void __handler(int n) {
	__exit_flag=1; 
}

//Gets the id of the server
int __id_Server(int id){
	char * srv="server";
	int srvid;
	if (id==SRV_ID){
		FILE * file=__open(srv,"r", SERVER_PATH);
		if(file==NULL){
			printf("%s\n","Error opening server id file in client");
		}
		while(fscanf(file,"%d\n", &srvid) != EOF) {;}
		return srvid;
	}else{
		return false;
	}

}