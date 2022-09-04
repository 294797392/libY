/***********************************************************************************
 * @ file    : Yfactory.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.08.31 19:33
 * @ brief   : ģ�鹤��
 * @ remark  ��
 ************************************************************************************/

#ifndef __YFACTORY_H__
#define __YFACTORY_H__

#include "Y.h"
#include "Ylist.h"
#include "Ymodule.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef void(*Yfactory_initialized)(Yfactory *factory);
    typedef void(*Yfactory_module_status_changed)(Yfactory *factory, Ymodule *m, Ymodule_status status);

    // ģ�����
    typedef struct Yfactory_s Yfactory;

    typedef struct Yfactory_options_s
    {
        Yfactory_initialized on_initialized;
        Yfactory_module_status_changed on_module_status_changed;
    }Yfactory_options;

    /*
     * ������
     * ʹ��options����һ������
     *
     * ������
     * @opts��ģ�鹤����ѡ��
     *
     * ����ֵ��
     * Yfactoryʵ��
     */
    YAPI Yfactory *Y_create_factory(Yfactory_options *opts);

    /*
     * ������
     * ʹ��һ��ģ���첽��ʼ��ģ�鹤��
     *
     * ������
     * @modules��Ҫ���ص�ģ���б�
     *
     * ����ֵ��
     * Yfactoryʵ��
     */
    YAPI void Y_setup_factory_async(Yfactory *factory, Ylist *modules);

    YAPI void Y_delete_factory(Yfactory *yf);

#ifdef __cplusplus
}
#endif

#endif