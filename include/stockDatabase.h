#ifndef STOCK_DB_H
#define STOCK_DB_H

#include "common.h"
#include "model.h"

typedef enum {
	NO_STOCK_FOR_ID = OK+1, STOCK_EXISTS
} DBErrors;

int saveStock(StockT stock);
int getStockByProductId(int productId, Stock stock);
int updateStock(StockT stock);
int deleteStock(StockT stock);

#endif