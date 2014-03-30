#include <stdio.h>
#include "../include/stockDB.h"
#include "../include/common.h"
#include "../include/model.h"

int main(void) {
	int testAns;

	StockT a,b,c;
	a.name = "pen";
	a.quantity = 100;

	b.name = "stapler";
	b.quantity = 500;

	saveStock(a);
	saveStock(b);

	testAns = getStockByProductName(a.name, &c);
	if (testAns == OK){
		printf("Name=%s, quantity=%d (OK)\n", a.name, a.quantity);
	} else {
		printf("Error (NOT OK) %d\n",testAns);
	}

	testAns = getStockByProductName("pencil", &c);
	if (testAns == NO_STOCK_FOR_NAME){
		printf("Expected error (OK)\n");
	} else {
		printf("Unexpected error (NOT OK): %d\n", testAns);
	}

	a.quantity = 99;
	testAns = updateStock(a);
	if(testAns != OK){
		printf("Update error: %d\n", testAns);
	}
	testAns = getStockByProductName("pen", &c);
	if(testAns == OK){
		printf("Name=%s, quantity=%d (OK)\n", a.name, a.quantity);
	} else {
		printf("Unexpected error (NOT OK): %d\n", testAns);
	}

	testAns = deleteStock(b);
	if(testAns != OK) {
		printf("Delete error: %d\n", testAns);
	}
	testAns = getStockByProductName("stapler", &c);
	if (testAns == NO_STOCK_FOR_NAME){
		printf("Error (OK)\n");
	}else {
		printf("NO Error (NOT OK): %d\n", testAns);
	}
	return 0;
}
