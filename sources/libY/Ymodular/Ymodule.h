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