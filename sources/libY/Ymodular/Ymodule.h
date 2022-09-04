/***********************************************************************************
 * @ file    : Ymodule.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.08.31 19:16
 * @ brief   : ģ��
 * @ remark  ��
 ************************************************************************************/

#ifndef __YMODULE_H__
#define __YMODULE_H__

#include "Y.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_STRING_SIZE                 2048

    typedef enum
    {
        YMOD_FLAGS_NONE,
        YMOD_FLAGS_DISABLED
    }Ymodule_flags;

    typedef enum
    {
        YMOD_STAT_UN_INITIALIZED,
        YMOD_STAT_INITIALIZED,
        YMOD_STAT_INIT_FAILED,
        YMOD_STAT_INITIALIZING,
        YMOD_STAT_INIT_EXCEPTION
    }Ymodule_status;

    typedef struct Ymodule_manifest_s
    {
        char *id;
        char *description;
        char *lib_path;

        int flags;

    }Ymodule_manifest;

    // ģ�����
    typedef struct Ymodule_s Ymodule;

    YAPI Ymodule *Y_create_module();

    /*
     * ������
     * ɾ��һ���¼�
     *
     * ������
     * @ye��Ҫɾ�����¼�
     */
    YAPI void Y_delete_module(Ymodule *ym);

#ifdef __cplusplus
}
#endif

#endif