#ifndef __ACAP_DRIVER_H__
#define __ACAP_DRIVER_H__

typedef struct acap_driver_s
{
    char *name;
    char *author;

    void*(*initialize)();
    void(*release)(void *driver_object);

    int(*start)(void *driver_object);
    void(*stop)(void *driver_object);

}acap_driver;

#endif
