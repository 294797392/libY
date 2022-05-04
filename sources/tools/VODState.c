#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "VODController.h"

struct VODState
{
    struct VODController Controller;
};

struct VODState *CreateVOD()
{
    struct VODState *state = (struct VODState*)calloc(1, sizeof(struct VODState));
}

int DeleteVOD(struct VODState *state)
{

}

int VOD_SetSurface(void *surface)
{

}

int VOD_RenderFrame(const char *frame, size_t frameSize)
{

}

