#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ylogbase.h"

typedef struct Yappender_s
{
	void *context;

	void *(*open)(const char *uri);
	void(*close)(void *ctx);
	void(*write)(void *ctx, const Ymsg *ymsg);
	void(*flush)(void *ctx);
}Yappender;
