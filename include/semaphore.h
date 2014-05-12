#ifndef _SEMAPHORE_
#define _SEMAPHORE_

#include "common.h"

void semaphore_init(int sem_num, boolean creat);
void semaphore_let(int sem_id);
void semaphore_stop(int sem_id);
void semaphore_destroy(int sem_id);
int semaphore_get_val(int sem_id);

#endif
