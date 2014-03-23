#include "../include/stockDatabase.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h> // for perror

#define DB_PATH	"stockDB.csv"

int save(StockT stock) {
	switch (getByProductId(stock.productId, &stock)) {
		case OK:
			return STOCK_EXISTS;
	}
	__write_new(stock);
	return OK;
}

int getByProductId(int productId, Stock stock) {
	struct StockT readStock;

	FILE * file = __open("r");
	if (file == NULL) {
		__init();
		return NO_STOCK_FOR_ID;
	}

	while(fscanf(file,"%d,%d\n", &(readStock.productId), &(readStock.stock)) != EOF
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

int update(StockT stock);

int remove(StockT stock);

void __init() {
	FILE * file = __open("a");
	fclose(file);
}

void __write_new(StockT stock) {
	FILE * file = __open("a");
	fprintf(file, "%d,%d\n", stock.productId, stock.quantity);
	fclose(file);
}

FILE * __open(const char * mode) {
	return fopen(DB_PATH,mode);
}