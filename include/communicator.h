#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

int write(int fromId, int toId, void * msg, int msgLen);
int read(int fromId, int toId, void * msg, int msgLen);

#endif