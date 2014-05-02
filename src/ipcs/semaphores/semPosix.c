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

void semaphore_create(int sem_id) {
	semaphore_destroy(sem_id);
	if (sem_open(__as_sem_name(sem_id), O_CREAT | O_EXCL | O_RDWR, 0666, 0) == SEM_FAILED) {
		fail("Sem failed");
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

void semaphore_show(int sem_id) {
	int val;
	if (sem_getvalue(__sem_get(__as_sem_name(sem_id)), &val) != -1) {
		printf("Sem val: %d\n", val);
	}
}

sem_t * __sem_get(string sem_name) {
	return sem_open(sem_name, O_RDWR);
}

string __as_sem_name(int sem_id) {
	void * buf = malloc(strlen(BASE_SEM_NAME)+floor(log10(sem_id))+2);
	if (buf == NULL) {
		fail("Could not allocate memory");
	}
	sprintf(buf, "/"BASE_SEM_NAME"%d", sem_id);
	return (string) buf;
}
