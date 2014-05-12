#include "../../include/communicator.h"
#include "../../include/utils.h"
#include "../../include/rdwrn.h"
#include "../../include/common.h"
#include "../../include/error.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#define _POSIX_SOURCE

#define INVALID -1
#define BUFFER_SIZE 100

#define ALL_RW S_IRWXU|S_IRWXG|S_IRWXO

#define SIGNAL_MSG_DATA_SIZE (SIGNAL_MSG_LEN+sizeof(int))
#define SIGNAL_MSG_LEN 200

#define SIGNAL_DIR "/tmp/so-signal"
#define SERVER_FILE "server"
#define CLIENT_FILE "client"

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
	mkdir(SIGNAL_DIR, ALL_RW);
	FILE * file=__open(SERVER_FILE, "w+r", SIGNAL_DIR);
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
	char ipc_name[20];
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

	sprintf(ipc_name, "%d", from_id);
	//Creates the "from" file to pass the information 
	FILE * file=__open(ipc_name,"r",SIGNAL_DIR);
	if(file==NULL){
		ret=__write_new(ipc_name);
	}else{
		fclose(file);
	}
	printf("From id %d To id %d\n",from_id,to_id);
	if(to_id!=INVALID){
		printf("%s\n","Client");
		//Client
		int clientid=getpid();
		FILE * file2=__open(CLIENT_FILE, "w+r", SIGNAL_DIR);
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
	int ret;
	int srvid=__id_Server(from_id);

	if(srvid!=false){
		from_id=srvid;
	}else{
		srvid=__id_Server(to_id);
		if(srvid!=false){
			to_id=srvid;
		}
	}

	char ipc_name[20];
	sprintf(ipc_name, "%d", to_id);
	FILE * file=__open(ipc_name, "r+w",SIGNAL_DIR);
	verify(file != NULL, "Null file after open");

	int fd = fileno(file);

	verify((ret = fwriten(fd, &len, sizeof(int))) != -1, "Send error");
	verify((ret = fwriten(fd, buf, len)) != -1, "Send error");

	printf("%s\n","Sending signal");
	kill(to_id,SIGUSR1);
	return fclose(file);
}

//Reads from_id file
int ipc_recv(int from_id, void * buf, int len){
	sigset_t mask, oldmask;
	char ipc_name[20];
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

		sprintf(ipc_name, "%d", from_id);

		FILE * file = __open(ipc_name, "r+",SIGNAL_DIR);
		verify(file != NULL, "Null file after open");

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
	char ipc_name[20];

	switch(from_id) {
	case SRV_ID:
		return OK;
	default:
		sprintf(ipc_name, "%d", from_id);
		verify(unlink(__get_path(ipc_name, SIGNAL_DIR)) == 0, "Unexpected delete error");
		verify(unlink(__get_path(CLIENT_FILE, SIGNAL_DIR)) == 0, "Unexpected delete error");
		return OK;
	}
}

//Delete server file.
int ipc_close(int from_id){
	int srvid=__id_Server(from_id);
	from_id=srvid;
	char ipc_name[20];
	sprintf(ipc_name, "%d", from_id);
	printf("%s\n", __get_path(ipc_name, SIGNAL_DIR));
	verify(unlink(__get_path(ipc_name, SIGNAL_DIR)) == 0, "Unexpected delete error");
	verify(unlink(__get_path(SERVER_FILE, SIGNAL_DIR)) == 0, "Unexpected delete error");
	verify(rmdir(SIGNAL_DIR) == 0, "Could not remove ipc directory");
	return OK;
}

int __write_new(char * id) {
	FILE * file = __open(id, "w+",SIGNAL_DIR);
	verify(file != NULL, "__write_new file null");
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
	char * srv=SERVER_FILE;
	int srvid;
	if (id==SRV_ID){
		FILE * file=__open(srv,"r", SIGNAL_DIR);
		if(file==NULL){
			printf("%s\n","Error opening server id file in client");
		}
		while(fscanf(file,"%d\n", &srvid) != EOF) {;}
		return srvid;
	}else{
		return false;
	}

}