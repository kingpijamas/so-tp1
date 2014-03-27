#include "../include/stockDB.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h> // for perror and remove
#include <stdlib.h> // for malloc

#define DB_ROOT_PATH "stockDB"
#define MAX_LENGHT 50

void __init_db();
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
	StockT readStock;
	readStock.name=(char*)malloc(MAX_LENGHT);

	FILE * file = __open(name, "r");
	if (file == NULL) {
		__init_db();
		return NO_STOCK_FOR_NAME;
	}

	while(fscanf(file,"%s ,%d\n", readStock.name, &(readStock.quantity)) != EOF
			&& name != readStock.name) {
;
	}
	fclose(file);

	// printf("%s\n",name);
	// printf("%s\n",readStock.name);
	// printf("%i\n",readStock.quantity);
	if (*name != *readStock.name) {
		printf("%s\n","no" );
		stock = NULL;
		return NO_STOCK_FOR_NAME;
	}
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
			return UPDATE_ERROR;
	}
}

int deleteStock(StockT stock) {
	char fname[32];
	sprintf(fname, "%s/%s", DB_ROOT_PATH, stock.name);
	if (remove(fname) == 0){
		return OK;
	}
	return DELETE_ERROR;
}

void __init_db() {
	// read/write/search permissions for owner and group, and with read/search permissions for others
	if (mkdir(DB_ROOT_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		// error ocurred!
	}
}

void __write_new(StockT stock) {
	FILE * file = __open(stock.name, "w");
	fprintf(file, "%s ,%d\n", stock.name, stock.quantity);
	fclose(file);
}

// Revisar sprintf con los cambios %s/%s
FILE * __open(char* name, const char * mode) {
	char fname[32];
	sprintf(fname, "%s/%s", DB_ROOT_PATH, name);
	return fopen(fname, mode);
}
