#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

int write(int fromId, int toId, void * msg, int length);

int read(int fromId, int toId, void * msg, int length);

#endif