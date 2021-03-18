#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "av/acap/drivers/acap_driver.h"

static void *mci_acap_init()
{
    return NULL;
}

static void mci_acap_release(void *driver_object)
{}

static int mci_acap_start(void *driver_object)
{
    return 0;
}

static void mci_acap_stop(void *driver_object)
{

}

acap_driver mci_acap_driver = 
{
    .name = "mci_acap_driver",
    .author = "oheiheiheiheihei",

    .initialize = mci_acap_init,
    .release = mci_acap_release,

    .start = mci_acap_start,
    .stop = mci_acap_stop
};

