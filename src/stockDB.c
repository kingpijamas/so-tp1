#include "../include/stockDB.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h> // for perror and remove

#define TABLE_PATH DB_ROOT_PATH"/"TABLE_NAME
#define MAX_TUPLE_LENGTH 32

int __init();
void __write_new(StockT stock);
FILE * __open(char* name, const char * mode);
char * __get_path_to_tuple(char* name);

int saveStock(StockT stock) {
	switch (getStockByProductName(stock.name, &stock)) {
		case OK:
			return STOCK_EXISTS;
	}
	__write_new(stock);
	return OK;
}

int getStockByProductName(char* name, Stock stock) {
	int initVal;
	StockT readStock;

	FILE * file = __open(name, "r");
	if (file == NULL) {
		initVal = __init();
		return (initVal != OK)? initVal : NO_STOCK_FOR_NAME;
	}
	readStock.name = name;
	while(fscanf(file,"%d\n", &(readStock.quantity)) != EOF) {;}
	fclose(file);
	stock = &readStock;
	return OK;
}

int updateStock(StockT stock) {
	switch (getStockByProductName(stock.name, &stock)) {
		case NO_STOCK_FOR_NAME:
			return NO_STOCK_FOR_NAME;
		case OK:
			__write_new(stock);
			return OK;
		default:
			return UNEXPECTED_UPDATE_ERROR;
	}
}

int deleteStock(StockT stock) {
	char fname[MAX_TUPLE_LENGTH];
	sprintf(fname, "%s/%s", TABLE_PATH, stock.name);
	if (remove(fname) != 0){
		return DELETE_ERROR;
	}
	return OK;
}

int __init() {
	// read/write/search permissions for owner and group, and with read/search permissions for others
	if (mkdir(DB_ROOT_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_DATABASE;
	}
	if (mkdir(TABLE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		&& errno != EEXIST) {
		return CANNOT_CREATE_TABLE;
	}
	return OK;
}

void __write_new(StockT stock) {
	FILE * file = __open(stock.name, "w");
	fprintf(file, "%d\n", stock.quantity);
	fclose(file);
}

// Revisar sprintf con los cambios %s/%s
FILE * __open(char* name, const char * mode) {
	char fname[MAX_TUPLE_LENGTH];
	sprintf(fname, "%s/%s", TABLE_PATH, name);
	return fopen(fname, mode);
}