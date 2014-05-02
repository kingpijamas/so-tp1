CC = gcc
SRC = src
MAIN = main

MODEL = $(SRC)/model
DB = $(SRC)/db
CLIENTS = $(SRC)/client
IPCS = $(SRC)/ipcs
UTILS = $(SRC)/utils
MSG = $(SRC)/msg
RUN = $(MAIN)/$(MAIN_FILE)

MAIN_FILE = tests/ipcTest.c
IPC = fifo.c
CLIENT = clientNoSrv.c

DIRS = $(MODEL) $(DB) $(UTILS) $(SERVER) $(MSG)
FILES = $(shell find $(DIRS) -name '*.c') $(CLIENTS)/$(CLIENT) $(IPCS)/$(IPC) $(RUN)

OUT_EXE = tp
COPTS = -Wall -g

tp:
	$(CC) $(COPTS) -o $(OUT_EXE) $(FILES)

clean:
	-rm -f $(OUT_EXE)