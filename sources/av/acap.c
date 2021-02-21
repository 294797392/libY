#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "av/av.h"
#include "av/acap.h"

struct acap_s
{
    void *driver;
    acap_config *config;
};

acap *acap_create(acap_config *config)
{
    acap *cap = (acap*)calloc(1, sizeof(acap));
    if(cap == NULL)
    {
        return NULL;
    }

    cap->config = config;

    return cap;
}

void acap_release(acap *cap)
{

}

