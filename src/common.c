#include "../include/common.h"
#include <stdlib.h>

void product_init(Product * product){
	(*product).name=malloc(MAX_NAME);
}