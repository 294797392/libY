#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Yerrno.h"
#include "Yfactory.h"

typedef struct demo_module_s
{
	int value;
}demo_module;

int demo_module_init(Ymodule *module, void **context)
{
	return YERR_SUCCESS;
}

void demo_module_release(Ymodule *module)
{

}
