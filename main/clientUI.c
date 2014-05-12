#include "../include/client.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

static void __quit();
static void __parse_input();
static string __get_err_str(int option);

int main(void) {
	signal(SIGPIPE, __quit);
	printf("Bienvenido al sistema de gestion de stock!\n");
	while(true) {
		__parse_input();
		getchar();
	}
}

void __parse_input() {
	int option, quantity, res;
	product_name name_buf;

	printf("Ingrese el numero de la opcion que desee:\n");
	printf("[1] Ver producto\n");
	printf("[2] Registrar producto nuevo\n");
	printf("[3] Depositar stock de un producto\n");
	printf("[4] Retirar stock de un producto\n");
	printf("[5] Dar de baja producto\n");
	printf("[6] Salir\n");
	scanf("%d", &option);

	printf("Ingrese el nombre del producto:\n");
	scanf("%s", name_buf);
	
	switch(option) {
	case 2:
	case 3:
	case 5:
		printf("Ingrese la cantidad:\n");
		scanf("%d", &quantity);
		break;
	case 6:
		__quit();
	}

	switch (option) {
	case 1:
		res = clt_show_product(name_buf);
		break;
	case 2:
		res = clt_add_product(name_buf, quantity);
		if (res == PRODUCT_ALREADY_INIT) {
			printf("Disculpe, ese producto ya esta registrado\n");
			return;
		}
		break;
	case 3:
		res = clt_deposit_product(name_buf, quantity);
		if (res == NEGATIVE_QUANTITY) {
			printf("Disculpe, solo puede ingresar cantidades positivas al sistema\n");
			return;
		}
		break;
	case 4:
		res = clt_remove_product(name_buf);
		break;
	case 5:
		res = clt_take_product(name_buf, quantity);
		if (res == NEGATIVE_QUANTITY) {
			printf("Disculpe, solo puede ingresar cantidades positivas al sistema\n");
			return;
		}
		break;
	}
	if (res != OK) {
		printf("Disculpe, hubo un error %s (%d)\n", __get_err_str(option), res);
	}
}

void __quit() {
	printf("Fin del programa. Hasta luego\n");
	exit(0);
}

string __get_err_str(int option) {
	switch(option){
	case 1:
		return "al intentar ver el producto";
	case 2:
		return "al intentar registrar el producto";
	case 3:
		return "al intentar depositar el producto";
	case 4:
		return "al intentar retirar el producto";
	case 5:
		return "al intentar dar de baja el producto";
	}
}