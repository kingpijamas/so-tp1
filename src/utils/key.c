#include "../../include/key.h"
#include <stdio.h>

#define KEY_FILE "/home/"

key_t key_get(int id) {
	int key = ftok(KEY_FILE, id);
	//printf("Key: %x\n", key);
	return key;
}