#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "../../include/utils.h"
#include "../../include/rdwrn.h"
#include "../../include/common.h"
#define SIGNALFILES_IPC_DIR "/tmp/so-signalfiles"
#define BUFFER_SIZE 100
static char buf[BUFFER_SIZE];
#define SERVER_INUSE=0

int ipc_init(int from_id){
	return OK;
}
int ipc_close(int from_id){
	return OK;
}

//Crea los archivos de from id y to id si no existen
int ipc_connect(int from_id, int to_id){
	char ipcname[20];
	int ret;
	sprintf(ipcname, "%d", from_id);
	// Necesito un semáforo, puedo perder el procesador después del while
	// while(SERVER_INUSE==TRUE){
	// 	;
	// }
	// SERVER_INUSE=TRUE;
	if((FILE * file=__open(ipcname,"r"))==NULL){
		ret=__write_new(ipcname);
	}
	sprintf(ipcname, "%d", to_id);
	if((file=__open(ipcname,"r"))==NULL){
		ret=__write_new(ipcname);
	}
	return ret;
}
int ipc_disconnect(int from_id, int to_id){
	return SERVER_INUSE=FALSE;
}
//escribe en el archivo de to_id
int ipc_send(int from_id, int to_id, void * buf, int len);

//lee el archivo de from_id
int ipc_rcv(int from_id, void * buf, int len);

void __write_new(char * id) {
	FILE * file = __open(id, "w+");
	fd=fileno(file);
	fclose(file);
}

FILE * __open(char * name, const string mode) {
	return fopen(__get_path(name), mode);
}

string __get_path(char * id) {
	sprintf(buf, "%s/%s", SIGNALFILES_IPC_DIR, id); 
	return buf;
}