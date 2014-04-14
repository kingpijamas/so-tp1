#ifndef _COMMUNICATOR_
#define _COMMUNICATOR_

int ipc_init(int fromid);
//fromid toId from the processes
int ipc_send(int fromid, int toId, void * buf, int len);
//who called recv could easily go into the message (if necessary)
int ipc_recv(void * buf, int len, int toId);
int ipc_close(int fromid);

#endif
