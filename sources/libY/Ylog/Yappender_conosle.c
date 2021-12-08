﻿/***********************************************************************************
 * @ file    : Yappender_console.c
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2021.12.06 13.38
 * @ brief   : 用来向控制台输出日志的appender
 ************************************************************************************/

#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "Ylogbase.h"
#include "Yappender.h"

typedef struct Yconsole_s
{
	FILE *fd;
}Yconsole;

static void *open(const char *uri)
{
	Yconsole *console = (Yconsole *)calloc(1, sizeof(Yconsole));
	return console;
}

static void close(void *ctx)
{
	Yconsole *console = (Yconsole *)ctx;
}

static void write(void *ctx, const Ymsg *ymsg)
{
	Yconsole *console = (Yconsole *)ctx;
	fwprintf(stdout, ymsg->msg);
#ifdef Y_ENV_MINGW
	// mingw环境下不能每次都马上输出到控制台，这里每次都flush一下
	fflush(stdout);
#endif
}

static void flush(void *ctx)
{
	Yconsole *console = (Yconsole *)ctx;
	fflush(stdout);
}

struct Yappender_s Yappender_console =
{
	.open = open,
	.close = close,
	.write = write,
	.flush = flush
};


