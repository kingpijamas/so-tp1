#include "../../include/key.h"
#include <stdio.h>

#define KEY_FILE "/home/"

key_t key_get(char key_char) {
	int key = ftok(KEY_FILE, key_char);
	//printf("Key: %x\n", key);
	return key;
}