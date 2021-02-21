#ifndef __AVCAP_H__
#define __AVCAP_H__

#include "av/av.h"

typedef enum
{
    ACAP_DRV_DSOUND,
    ACAP_DRV_APLAY
}ACAP_DRIVER;

typedef struct acap_cofnig_s 
{
    ACAP_DRIVER driver_type;

}acap_config;

typedef struct acap_s acap;

AVAPI acap *acap_create(acap_config *config);
AVAPI void acap_release(acap *cap);

#endif