#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include "../../include/utils.h"
#include "../../include/rdwrn.h"
#include "../../include/common.h"
#define INVALID -1
#define SIGNALFILES_IPC_DIR "src"
#define BUFFER_SIZE 100
#define UNEXPECTED_DELETE_ERROR 2
#define _POSIX_SOURCE
static char buf[BUFFER_SIZE];
static void __handler(int n);
static string __get_path(char * id);
static int __write_new(char * id) ;
static FILE * __open(char * name, const string mode);
int ipc_connect(int from_id, int to_id);
int exit_flag;


//inicia el file del server
int ipc_init(int from_id){
	ipc_connect(from_id, INVALID);
	return OK;
}

//Borro el file del server
int ipc_close(int from_id){
	char ipcname[20];
	sprintf(ipcname, "%d", from_id);
	if (remove(__get_path(ipcname)) != 0){
		return UNEXPECTED_DELETE_ERROR;
	}
	return OK;
}

//Crea los archivos de from id si no existen
int ipc_connect(int from_id, int to_id){
	char ipcname[20];
	int ret;
	sprintf(ipcname, "%d", from_id);
	FILE * file=__open(ipcname,"r");
	if(file==NULL){
		ret=__write_new(ipcname);
		printf("Iniciado %s\n",ipcname);
		return ret;
	}
	fclose(file);
	return ret;
}

//borro el file del cliente 
int ipc_disconnect(int from_id, int to_id){
	char ipcname[20];
	sprintf(ipcname, "%d", from_id);
	if (remove(__get_path(ipcname)) != 0){
		return UNEXPECTED_DELETE_ERROR;
	}
	return OK;
}

//escribe en el archivo de to_id. Mando señal para despertar.
int ipc_send(int from_id, int to_id, void * buf, int len){
	char ipcname[20];
	sprintf(ipcname, "%d", to_id);
	// printf("%s\n","Before open");
	FILE * file=__open(ipcname, "r+w");
	if(file==NULL){
		printf("After open, file null. ipcname:%s\n to_id:%d",ipcname,to_id);
		return ERROR;
	}
	int ret=fwriten(fileno(file),buf,len); //atomico. Si lo interrumpe una señal sigue hasta escribir los n.
	if(ret==-1){
		printf("%s\n","Fwriten returning -1");
		return ERROR;
	}
	printf("%s\n","Sending signal");
	kill(to_id,SIGUSR1);
	return fclose(file);
}

//lee el archivo de from_id
int ipc_rcv(int from_id, void * buf, int len){
	char ipcname[20];
	printf("Exit flag %d\n",exit_flag);
	// signal(SIGUSR1,__handler);
	struct sigaction act; 
	memset (&act, '\0', sizeof(act));
	act.sa_handler = &__handler;
	if (sigaction(SIGUSR1, &act, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}
	while(!exit_flag){
		;
	}
	printf("%s\n","After signal" );
	sprintf(ipcname, "%d", from_id);
	FILE * file=__open(ipcname, "r+");
	printf("ipc name %s\n",ipcname);
	// printf("%s\n","1");
	if(file==NULL){
		printf("%s\n","Null"); //QUE CARAJO DE DONDE SALIS 
		return ERROR;
	}
	// printf("%s\n","2");
	// printf("%s\n","3");
	freadn(fileno(file),buf,len);
	exit_flag=0;
	return OK;
}

int __write_new(char * id) {
	FILE * file = __open(id, "w+");
	if (file==NULL){
		printf("%s\n","__write_new file null");
		return ERROR;
	}
	printf("FD %d\n",fileno(file));
	close(fileno(file));
	return OK;
}

FILE * __open(char * name, const string mode) {
	return fopen(__get_path(name), mode);
}

string __get_path(char * id) {
	sprintf(buf, "%s/%s", SIGNALFILES_IPC_DIR, id); 
	printf("BUF %s\n",buf);
	return buf;
}

void __handler(int n) {
	exit_flag=1; 
	printf("%s\n","I'm a handler");	;
}