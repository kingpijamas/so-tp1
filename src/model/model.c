#include "../../include/model.h"
#include <stdlib.h>

void product_init(Product * product){
	(*product).name=malloc(PRODUCT_MAX_NAME_LEN);
}