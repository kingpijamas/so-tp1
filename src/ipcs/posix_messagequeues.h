#ifndef _POSIX_MSGQUE_
#define _POSIX_MSGQUE_

#include "common.h"
#include <mqueue.h>

#define CLTNAMESIZE 100
#define MAX_MSG 10
#define MSG_PRIORITY 0
#define MSG_SIZE 50
// #define PACKAGE_SIZE 50

struct {
	long fromId;
	char msg[MSG_SIZE];
} msgQueuePackage;

// #define MSG_SIZE (PACKAGE_SIZE-sizeof(long int))