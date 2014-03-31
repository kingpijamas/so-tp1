#ifndef _COMMUNICATOR_
#define _COMMUNICATOR_

int ipc_init(int fromId);
//fromId toId from the processes
int ipc_send(int fromId, int toId, void * buf, int len);
//who called recv could easily go into the message (if necessary)
int ipc_recv(void * buf, int len);
int ipc_close(int fromId);

#endif
