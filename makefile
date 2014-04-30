CC = gcc
SRC = src
MAIN = main

MODEL = $(SRC)/model
DB = $(SRC)/db
CLIENTS = $(SRC)/client
SERVER = $(SRC)/server
IPCS = $(SRC)/ipcs
SEMS = $(IPCS)/semaphores
UTILS = $(SRC)/utils
MSG = $(SRC)/msg
RUN = $(MAIN)/$(MAIN_FILE)

MAIN_FILE = tests/ipcTest.c
IPC = fifo.c
CLIENT = client.c
SEM = semPosix.c

DIRS = $(MODEL) $(DB) $(UTILS) $(SERVER) $(MSG)
FILES = $(shell find $(DIRS) -name '*.c') $(CLIENTS)/$(CLIENT) $(IPCS)/$(IPC) $(SEMS)/$(SEM) $(RUN)

OUT_EXE = tp
COPTS = -Wall -g

tp:
	$(CC) $(COPTS) -o $(OUT_EXE) $(FILES) -pthread -lm

clean:
	-rm -f $(OUT_EXE)