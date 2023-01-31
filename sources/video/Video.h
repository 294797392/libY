#ifndef __VIDEO_H__
#define __VIDEO_H__

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct VideoFilter VideoFilter;
    typedef enum VideoFilters VideoFilters;

    enum VideoFilters
    {
        VIDEO_FILTER_FLIPX,
        VIDEO_FILTER_FLIPY
    };

    struct VideoFilter
    {
        VideoFilters Filter;
        char Parameter[1024];
    };

#ifdef __cplusplus
}
#endif

#endif