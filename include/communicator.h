#ifndef _COMMUNICATOR_
#define _COMMUNICATOR_

int ipc_init(int from_id);
//from_id to_id from the processes
int ipc_send(int from_id, int to_id, void * buf, int len);
//who called recv could easily go into the message (if necessary)
int ipc_recv(void * buf, int len);
int ipc_close(int from_id);

#endif
