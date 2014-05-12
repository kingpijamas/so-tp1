#include "../../../include/semaphore.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../../../include/error.h"

#define BASE_SEM_NAME "so-sem-posix-"

static sem_t * __sem_get();
//static void __sem_show_value();
static string __as_sem_name(int sem_id);
static void __semaphore_create(int sem_id);

void semaphore_init(int sem_n, boolean creat) {
	int i;
	if (!creat) {
		return;
	}
	for(i=0; i<sem_n; i++){
		__semaphore_create(i);
	}
}

void semaphore_let(int sem_id) {
	sem_post(__sem_get(__as_sem_name(sem_id)));
}

void semaphore_stop(int sem_id) {
	sem_wait(__sem_get(__as_sem_name(sem_id)));
}

void semaphore_destroy(int sem_id) {
	sem_unlink(__as_sem_name(sem_id));
}

int semaphore_get_val(int sem_id) {
	int val;
	verify(sem_getvalue(__sem_get(__as_sem_name(sem_id)), &val) != -1, "Error getting semaphore value");
	return val;
}

void __semaphore_create(int sem_id) {
	semaphore_destroy(sem_id);
	if (sem_open(__as_sem_name(sem_id), O_CREAT | O_EXCL | O_RDWR, 0666, 0) == SEM_FAILED) {
		fail("Failure creating semaphore");
	}
}

sem_t * __sem_get(string sem_name) {
	return sem_open(sem_name, O_RDWR);
}

string __as_sem_name(int sem_id) {
	void * buf = malloc(200);
	if (buf == NULL) {
		fail("Could not allocate memory");
	}
	sprintf(buf, "/"BASE_SEM_NAME"%d", sem_id);
	return (string) buf;
}
