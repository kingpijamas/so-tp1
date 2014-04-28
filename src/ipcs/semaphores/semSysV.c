#include "../../../include/semaphore.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define SEM_ROOT_PATH "/tmp/so-sem"
#define BASE_SEM_NAME "so-sem-sysv"
#define SEM_PATH SEM_ROOT_PATH"/"BASE_SEM_NAME

static sem_t * __sem_get();
static void __sem_show_value();

static sem_id sem = NULL;

void semaphore_create() {
	if (mkdir(SEM_ROOT_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_DATABASE;
	}
	if (mkdir(TABLE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_TABLE;
	}


	key_t key = ftok(SEM_PATH, 'A');

	int errnost;
	if (sem_open(SEM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666, 0) == SEM_FAILED) {
		errnost = errno;
		printf("Sem failed: %d\n", errnost);
	}
}

void semaphore_up() {
	printf("UP\n");
	printf("Before:");
	__sem_show_value();
	sem_post(__sem_get());
	printf("After:");
	__sem_show_value();
}

void semaphore_down() {
	printf("DOWN\n");
	printf("Before:");
	__sem_show_value();
	sem_wait(__sem_get());
	printf("After:");
	__sem_show_value();
}

void semaphore_destroy() {
	sem_unlink(SEM_NAME);
}

void __sem_show_value() {
	int val;
	if (sem_getvalue(__sem_get(), &val) != -1) {
		printf("Sem: val=%d\n", val);
	}
}

sem_t * __sem_get() {
	if (sem != NULL) {
		return sem;
	}
	return sem_open(SEM_NAME, O_RDWR);
}