CC = gcc
SRC = src
#MODE = 
FILES = $(shell find $(SRC) -name '*.c') 
#$(shell find $(MODE) -name '*.c')
OUT_EXE = tp
COPTS = -Wall -g

tp:
	$(CC) $(COPTS) -o $(OUT_EXE) $(FILES)

clean:
	-rm -f $(OUT_EXE)