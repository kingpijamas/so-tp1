#ifndef _SEMAPHORE_
#define _SEMAPHORE_

#include "common.h"

void semaphore_create(int sem_id);
void semaphore_let(int sem_id);
void semaphore_stop(int sem_id);
void semaphore_destroy(int sem_id);
void semaphore_show(int sem_id);

#endif
