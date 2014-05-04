#include <stdio.h>
#include <stdlib.h>
#include "../../include/client.h"
#define MAX_LENGTH 50
void __return (int ret);

int main(){

	db_init();
	char product[MAX_LENGTH];
	int quantity,ret;
	printf("%s\n","Insert name of new product");
	gets(product);
	printf("%s\n","Insert quantity");
	scanf("%i",&quantity);
	getchar();
	
	ret=clt_add_product(product,quantity);
	__return(ret);
	printf("\n");

	printf("%s\n","Showing Product");
	ret=clt_show_product(product);
	__return(ret);
	printf("\n");

	printf("Insert number of %s to deposit ", product);
	scanf("%i",&quantity);getchar();
	ret=clt_deposit_product(product,quantity);
	__return(ret);
	// ret=clt_show_product(product);
	// __return(ret);
	// printf("\n");

	// printf("Insert number of %s to take ", product);
	// scanf("%i",&quantity); getchar();
	// ret=clt_take_product(product,quantity);
	// __return(ret);
	// ret=clt_show_product(product);
	// __return(ret);
	// printf("\n");

	printf("Deleting %s \n",product);
	ret=clt_remove_product(product);
	__return(ret);

	return 1;
}

void __return (int ret){
	switch(ret){
		case OK:
			printf("%s\n","OK");
			break;
		case PRODUCT_ALREADY_INIT:
			printf("%s\n","Product already init");
			break;
		case NEGATIVE_QUANTITY:
			printf("%s\n","Negative quantity");
			break;
		default:
			break;
	}
}