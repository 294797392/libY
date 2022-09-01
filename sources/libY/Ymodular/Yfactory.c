#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Y.h"
#include "Yfactory.h"

Yfactory *Y_create_factory(const YCHAR *config_path)
{
	FILE *f = fopen(config_path, "r");
}

void Y_delete_factory(Yfactory *yf)
{
}

