so-tp1
======

IPCs

To compile, use ./mk.sh ipc_base_name semaphore_base_name

i.e.:
	COMMAND					|	IPC
	=============================================================
	./mk.sh fifo			|	fifo
	./mk.sh msgqPosix		|	message queue (posix)
	./mk.sh msgqSysV		|	message queue (sysv)
	./mk.sh shm	semPosix	|	shared memory with posix semaphores
	./mk.sh shm semSysV		|	shared memory with sysv semaphores
	./mk.sh signals			|	files and signals
	./mk.sh socket			|	sockets

NOTES:
	clt-no-srv will be generated in every case, although it doesn't use any ipc