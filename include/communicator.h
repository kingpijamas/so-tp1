#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

//fromId toId from the processes
int send(int fromId, int toId, void * buf, int len);

//who called recv could easily go into the message (if necessary)
int recv(void * buf, int len);

#endif
