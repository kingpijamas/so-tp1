#include <stdio.h>
#include "../include/productDB.h"
#include "../include/common.h"
#include "../include/model.h"

int main(void) {
	int testAns;

	ProductT a,b,c;
	a.name = "pen";
	a.quantity = 100;

	b.name = "stapler";
	b.quantity = 500;

	saveProduct(a);
	saveProduct(b);

	testAns = get_product_by_name(a.name, &c);
	if (testAns == OK){
		printf("Name=%s, quantity=%d (OK)\n", a.name, a.quantity);
	} else {
		printf("Error (NOT OK) %d\n",testAns);
	}

	testAns = get_product_by_name("pencil", &c);
	if (testAns == NO_STOCK_FOR_NAME){
		printf("Expected error (OK)\n");
	} else {
		printf("Unexpected error (NOT OK): %d\n", testAns);
	}

	a.quantity = 99;
	testAns = updateProduct(a);
	if(testAns != OK){
		printf("Update error: %d\n", testAns);
	}
	testAns = get_product_by_name("pen", &c);
	if(testAns == OK){
		printf("Name=%s, quantity=%d (OK)\n", a.name, a.quantity);
	} else {
		printf("Unexpected error (NOT OK): %d\n", testAns);
	}

	testAns = deleteProduct(b);
	if(testAns != OK) {
		printf("Delete error: %d\n", testAns);
	}
	testAns = get_product_by_name("stapler", &c);
	if (testAns == NO_STOCK_FOR_NAME){
		printf("Error (OK)\n");
	}else {
		printf("NO Error (NOT OK): %d\n", testAns);
	}
	return 0;
}
