#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Yappender.h"

typedef struct Yfile_ctx_s
{
    FILE *fd;
}Yfile_ctx;

static void *open(const char *uri)
{
    Yfile_ctx *ctx = (Yfile_ctx*)calloc(1, sizeof(Yfile_ctx));
    if((ctx->fd = fopen(uri, "rb")) == NULL)
    {
        free(ctx);
        return NULL;
    }
    return ctx;
}

static void close(void *pctx)
{
    Yfile_ctx *ctx = (Yfile_ctx*)pctx;
    fclose(ctx->fd);
    free(ctx);
}

static void write(void *pctx, const Ymsg *ymsg)
{

}

static void flush(void *pctx)
{
    Yfile_ctx *ctx = (Yfile_ctx*)pctx;
    fflush(ctx->fd);
}

struct Yappender_s Yappender_file = 
{
    .open = open,
    .close = close,
    .write = write,
    .flush = flush
};



