#include "../include/stockDB.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h> // for perror and remove

#define DB_ROOT_PATH "stockDB"

void __init_db();
void __write_new(StockT stock);
FILE * __open(int tupleId, const char * mode);
char * __get_path_to_tuple(int tupleId);

int saveStock(StockT stock) {
	switch (getStockByProductId(stock.productId, &stock)) {
		case OK:
			return STOCK_EXISTS;
	}
	__write_new(stock);
	return OK;
}

int getStockByProductId(int productId, Stock stock) {
	StockT readStock;

	FILE * file = __open(productId, "r");
	if (file == NULL) {
		__init_db();
		return NO_STOCK_FOR_ID;
	}

	while(fscanf(file,"%d,%d\n", &(readStock.productId), &(readStock.quantity)) != EOF
			&& productId != readStock.productId) {
		;
	}
	fclose(file);

	if (productId != readStock.productId) {
		stock = NULL;
		return NO_STOCK_FOR_ID;
	}
	stock = &readStock;
	return OK;
}

int updateStock(StockT stock) {
	switch (getStockByProductId(stock.productId, &stock)) {
		case NO_STOCK_FOR_ID:
			return NO_STOCK_FOR_ID;
		case OK:
			__write_new(stock);
			return OK;
		default:
			return UPDATE_ERROR;
	}
}

int deleteStock(StockT stock) {
	char fname[32];
	sprintf(fname, "%s/%d", DB_ROOT_PATH, stock.productId);
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
	FILE * file = __open(stock.productId, "w");
	fprintf(file, "%d,%d\n", stock.productId, stock.quantity);
	fclose(file);
}

FILE * __open(int productId, const char * mode) {
	char fname[32];
	sprintf(fname, "%s/%d", DB_ROOT_PATH, productId);
	return fopen(fname, mode);
}
