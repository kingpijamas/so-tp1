#ifndef STOCK_DB_H
#define STOCK_DB_H

#include "common.h"
#include "model.h"

typedef enum {
	NO_PRODUCT_FOR_ID = OK+1
} DBErrors;

int save(StockT stock);
StockT getByProductId(int productId);
int update(StockT stock);
int remove(StockT stock);

#endif