CC = gcc
SRC = src

MODEL = $(SRC)/model
DB = $(SRC)/db
CLIENT = $(SRC)/client
IPCS = $(SRC)/ipcs
UTILS = $(SRC)/utils
MAIN = $(SRC)/$(RUN)

RUN = tests/ipcTest.c
IPC = fifo.c

DIRS = $(MODEL) $(DB) $(CLIENT) $(UTILS)
FILES = $(shell find $(DIRS) -name '*.c') $(IPCS)/$(IPC) $(MAIN)

OUT_EXE = tp
COPTS = -Wall -g

tp:
	$(CC) $(COPTS) -o $(OUT_EXE) $(FILES)

clean:
	-rm -f $(OUT_EXE)