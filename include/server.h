#include "communicator.h"

int rmDB(int fromId, int toId);
int writeDB(int fromId, int toId, void * msg, int msgLen);
int readDB(int fromId, int toId, void * msg, int msgLen);