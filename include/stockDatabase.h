#ifndef STOCK_DB_H
#define STOCK_DB_H

#include "common.h"
#include "model.h"

typedef enum {
	NO_STOCK_FOR_ID = OK+1, STOCK_EXISTS
} DBErrors;

int save(StockT stock);
int getByProductId(int productId, Stock stock);
int update(StockT stock);
int remove(StockT stock);

#endif