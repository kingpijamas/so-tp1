#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

//fromId toId from the processes
int send(int fromId, int toId, void * msg, int msgLen);
int recieve(int fromId, int toId, void * msg, int msgLen);

#endif
