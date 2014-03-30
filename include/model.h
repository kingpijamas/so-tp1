#ifndef MODEL_H
#define MODEL_H

#define NAME_LEN 8

typedef struct {
	//String of size == NAME_LEN
	char[NAME_LEN+1] name;
	int quantity;
} Product;

#endif
