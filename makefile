CC = gcc
SRC = src
MAIN = main

MODEL = $(SRC)/model
DBS = $(SRC)/db
CLIENTS = $(SRC)/client
SERVER = $(SRC)/server
IPCS = $(SRC)/ipcs
SEMS = $(IPCS)/semaphores
UTILS = $(SRC)/utils
MSG = $(SRC)/msg

RUN = tests/ipcTest.c
IPC = fifo.c
CLIENT = client.c
SEM = semPosix.c
DB = productDB.c

DIRS = $(MODEL) $(UTILS) $(SERVER) $(MSG)
FILES = $(shell find $(DIRS) -name '*.c') $(CLIENTS)/$(CLIENT) $(DBS)/$(DB) $(IPCS)/$(IPC) $(SEMS)/$(SEM) $(MAIN)/$(RUN)

OUT_EXE = tp
COPTS = -Wall -g

tp:
	$(CC) $(COPTS) -o $(OUT_EXE) $(FILES) -pthread -lmrt

clean:
	-rm -f $(OUT_EXE)
