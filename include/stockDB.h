#ifndef STOCK_DB_H
#define STOCK_DB_H

#include "common.h"
#include "model.h"

typedef enum {
	NO_STOCK_FOR_NAME = OK+1, STOCK_EXISTS, DELETE_ERROR, UPDATE_ERROR
} DBErrors;

/* Saves an element on the DB. See model.h for StockT. */
int saveStock(StockT stock);
/* Reads the stock of a certain product identified by the DB name*/
int getStockByProductName(char* name, Stock stock);
/* Changes the stock for an element of the DB*/
int updateStock(StockT stock);
/* Removes an element from the DB*/
int deleteStock(StockT stock);

#endif
