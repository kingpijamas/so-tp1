#include <stdio.h>
#include "../include/stockDB.h"
#include "../include/common.h"
#include "../include/model.h"

int main(void) {
	StockT a,b,c;
	a.name = "pen";
	a.quantity = 100;

	b.name = "stapler";
	b.quantity = 500;

	saveStock(a);
	// saveStock(b);

	/*if (getStockByProductName(a.name, &c) != NO_STOCK_FOR_NAME){
		printf("Name=%s, quantity=%d\n", a.name, a.quantity);
	} else {
		printf("Error (NOT OK)\n");
	}*/
	/*if (getStockByProductId(3, &c) == NO_STOCK_FOR_ID){
		printf("Error (OK)\n");
	}else {
		printf("NO Error (NOT OK)\n");
	}

	a.quantity = 99;
	updateStock(a);
	if (getStockByProductId(1, &c) != NO_STOCK_FOR_ID){
		printf("productId=%d, quantity=%d\n", a.productId, a.quantity);
	} else {
		printf("Error (NOT OK)\n");
	}

	deleteStock(b);
	if (getStockByProductId(2, &c) == NO_STOCK_FOR_ID){
		printf("Error (OK)\n");
	}else {
		printf("NO Error (NOT OK)\n");
	}*/
	return 0;
}