CC = gcc
SRC = src
MAIN = main

MODEL = $(SRC)/model
DB = $(SRC)/db
CLIENT = $(SRC)/client
SERVER = $(SRC)/server
IPCS = $(SRC)/ipcs
UTILS = $(SRC)/utils
MSG = $(SRC)/msg
RUN = $(MAIN)/$(MAIN_FILE)

MAIN_FILE = tests/ipcTest.c
IPC = fifo.c

DIRS = $(MODEL) $(DB) $(CLIENT) $(UTILS) $(SERVER) $(MSG)
FILES = $(shell find $(DIRS) -name '*.c') $(IPCS)/$(IPC) $(RUN)

OUT_EXE = tp
COPTS = -Wall -g

tp:
	$(CC) $(COPTS) -o $(OUT_EXE) $(FILES)

clean:
	-rm -f $(OUT_EXE)