#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include "../../include/utils.h"
#include "../../include/rdwrn.h"
#include "../../include/common.h"
#include "../../include/communicator.h"
#define INVALID -1
#define SIGNALFILES_IPC_DIR "src"
#define BUFFER_SIZE 100
#define UNEXPECTED_DELETE_ERROR 2
#define _POSIX_SOURCE
static char buf[BUFFER_SIZE];
static void __handler(int n);
static string __get_path(char * name, const char * path);
static int __write_new(char * id) ;
static FILE * __open(char * name, const string mode, const char * path);
static int __id_Server(int id);
int ipc_connect(int from_id, int to_id);
int exit_flag;

#define DB_ROOT_PATH "src"
#define SERVER_DIR "server"
#define CLIENT_DIR "client"
#define SERVER_PATH	DB_ROOT_PATH"/"SERVER_DIR
#define CLIENT_PATH	DB_ROOT_PATH"/"CLIENT_DIR

//Only the server uses init & close.
int ipc_init(int from_id){
	char * srv="server";
	if(from_id==SRV_ID){
		from_id=getpid();
	}
	FILE * file=__open(srv,"w+r",SERVER_PATH);
	if(file==NULL){
		printf("%s\n","Error opening server id file");
	}
	fprintf(file, "%d\n", from_id);
	fclose(file);

	ipc_connect(from_id, INVALID);
	return OK;
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

//Create the file from_id if it does not exist
int ipc_connect(int from_id, int to_id){	
	char * clt="client";
	char ipcname[20];
	int ret=OK;

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
		FILE * file2=__open(clt,"w+r",CLIENT_PATH);
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

//Delete client file
int ipc_disconnect(int from_id, int to_id){
	char ipcname[20];
	sprintf(ipcname, "%d", from_id);
	if (remove(__get_path(ipcname,SIGNALFILES_IPC_DIR)) != 0){
		return UNEXPECTED_DELETE_ERROR;
	}
	return OK;
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
	if(file==NULL){
		printf("After open, file null. ipcname:%s\n to_id:%d",ipcname,to_id);
		return ERROR;
	}
	int ret=fwriten(fileno(file),buf,len); //atomic. 
	if(ret==-1){
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
	while(!exit_flag){
		sigsuspend (&oldmask);
	}
	sigprocmask (SIG_UNBLOCK, &mask, NULL);
	sprintf(ipcname, "%d", from_id);
	FILE * file=__open(ipcname, "r+",SIGNALFILES_IPC_DIR);
	if(file==NULL){
		printf("%s\n","Null"); 
		return ERROR;
	}
	freadn(fileno(file),buf,len);
	exit_flag=0;
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
	sprintf(buf, "%s/%s", path, name); //this should clear the buffer (verify!)
	printf("BUF %s\n",buf);
	return buf;
}

void __handler(int n) {
	exit_flag=1; 
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