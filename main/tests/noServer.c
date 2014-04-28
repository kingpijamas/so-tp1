#include <stdio.h>
#include <stdlib.h>
#include "../../include/client.h"
#define MAX_LENGTH 50
void __return (int ret);

int main(){
	char product[MAX_LENGTH];
	int quantity,ret;
	db_init();
	printf("%s\n","Insert name of new product");
	gets(product);
	printf("%s\n","Insert quantity");
	scanf("%i",&quantity);getchar();
	
	ret=add_product(product,quantity);
	__return(ret);
	printf("\n");

	printf("%s\n","Showing Product");
	ret=show_product(product);
	__return(ret);
	printf("\n");

	printf("Insert number of %s to deposit ", product);
	scanf("%i",&quantity);getchar();
	ret=deposit_product(product,quantity);
	__return(ret);
	ret=show_product(product);
	__return(ret);
	printf("\n");

	printf("Insert number of %s to take ", product);
	scanf("%i",&quantity); getchar();
	ret=take_product(product,quantity);
	__return(ret);
	ret=show_product(product);
	__return(ret);
	printf("\n");

	printf("Deleting %s \n",product);
	ret=remove_product(product);
	__return(ret);

	return 1;
}

void __return (int ret){
	switch(ret){
		case OK:
			printf("%s\n","OK");
			break;
		case DB_NOT_INITIALIZED:
			printf("%s\n","Data base not initialized");
			break;
		case CANNOT_CREATE_DATABASE:
			printf("%s\n","Cannot create data base");
			break;
		case CANNOT_CREATE_TABLE:
			printf("%s\n","Cannot create table");
			break;
		case NO_PRODUCT_FOR_NAME:
			printf("%s\n","No product for name");
			break;
		case PRODUCT_EXISTS:
			printf("%s\n","Product exist");
			break;
		case UNEXPECTED_DELETE_ERROR:
			printf("%s\n","Unexpected delete error");
			break;
		case UNEXPECTED_UPDATE_ERROR:
			printf("%s\n","Unexpected update error");
			break;
		default:
			break;
	}
}