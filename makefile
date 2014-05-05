CC = gcc
SRC = src
MAIN = main

MODEL = $(SRC)/model
DB = $(SRC)/db
CLIENTS = $(SRC)/client
SERVER = $(SRC)/server
IPCS = $(SRC)/ipcs
UTILS = $(SRC)/utils
MSG = $(SRC)/msg
RUN = $(MAIN)/$(MAIN_FILE)

MAIN_FILE = tests/ipcTestSignals.c
IPC = fifo.c
CLIENT = client.c

DIRS = $(MODEL) $(DB) $(UTILS) $(SERVER) $(MSG)
FILES = $(shell find $(DIRS) -name '*.c') $(CLIENTS)/$(CLIENT) $(IPCS)/$(IPC) $(RUN)

OUT_EXE = tp
COPTS = -Wall -g
MQUEUE= -lrt

tp:
	$(CC) $(COPTS) -o $(OUT_EXE) $(FILES) $(MQUEUE)

clean:
	-rm -f $(OUT_EXE)