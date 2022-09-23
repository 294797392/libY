#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ylog.h"

typedef struct Yappender_s
{
	char *type;

	void *context;

	void *(*open)(const char *uri);
	void(*close)(void *ctx);
	void(*write)(void *ctx, const Ymsg *ymsg);
	void(*flush)(void *ctx);
}Yappender;
