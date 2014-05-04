#include "../../../include/semaphore.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../../include/key.h"
#include "../../../include/error.h"

static int __update_sem(int sem_id, int change);
static void __wipe_sems();

static int __sems_id = -1, __sem_n = -1;

typedef union {
	int val;
	struct semid_ds * buf;
	ushort * array;
} __semun;

void semaphore_init(int sem_n, boolean creat) {
	int flags = 0666;
	string error_text = "Failure initializing semaphore(s)";
	if (creat) {
		semaphore_destroy(0);
		error_text = "Failure creating semaphore(s)";
		flags = flags | IPC_CREAT | O_EXCL;
	}
	verify((__sems_id = semget(key_get('S'), sem_n, flags)) != -1, error_text);
	__sem_n = sem_n;
	if (creat) {
		__wipe_sems();
	}
}

void semaphore_let(int sem_id) {
	//printf("let: __sems_id:%d, __sem_n:%d, sem_id:%d\n", __sems_id, __sem_n, sem_id);
	verify(__update_sem(sem_id, +1) != -1, "Failure unlocking semaphore");
}

void semaphore_stop(int sem_id) {
	//printf("stop: __sems_id:%d, __sem_n:%d, sem_id:%d\n", __sems_id, __sem_n, sem_id);
	verify(__update_sem(sem_id, -1) != -1, "Failure waiting at semaphore");
}

void semaphore_destroy(int sem_id) {
	semctl(__sems_id, 0, IPC_RMID);
	__sems_id = __sem_n = -1;
}

void semaphore_show(int sem_id) {
	printf("Sem val: %d\n", semctl(__sems_id, sem_id, GETVAL));
}

int __update_sem(int sem_id, int change) {
	struct sembuf arg;
	arg.sem_num = sem_id;
	arg.sem_op = change;
	arg.sem_flg = SEM_UNDO; // seems nice to add this one
	return semop(__sems_id, &arg, 1);
}

void __wipe_sems() {
	ushort vals[__sem_n];
	__semun arg;

	memset(&vals, 0, __sem_n);
	arg.array = vals;

	semctl(__sems_id, 0, SETALL, &arg);
}