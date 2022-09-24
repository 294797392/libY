#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cJSON.h>

#include "Ylog.h"

typedef struct Yappender_s
{
	char *type;

	void *context;

	void *(*open)(cJSON *config);
	void(*close)(void *ctx);
	void(*write)(void *ctx, const Ymsg *ymsg);
	void(*flush)(void *ctx);
}Yappender;
