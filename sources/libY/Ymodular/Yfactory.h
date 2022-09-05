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

    typedef struct Yfactory_s Yfactory;

    typedef void(*Yfactory_initialized)(Yfactory *factory);
    typedef void(*Yfactory_module_status_changed)(Yfactory *factory, Ymodule *m, Ymodule_status status);

    typedef struct Yfactory_options_s
    {
        Yfactory_initialized on_initialized;
        Yfactory_module_status_changed on_module_status_changed;

        // ��ĳ��ģ���ʼ��ʧ�ܵ�ʱ���Զ����Գ�ʼ���ļ��ʱ��
        // ��λ����
        int initial_interval;
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
     * @modules��Ҫ���ص�ģ���б�, Ymodule_manifest�б�
     *
     * ����ֵ��
     * Yfactoryʵ��
     */
    YAPI int Y_setup_factory_async(Yfactory *factory, Ylist *manifests);

    YAPI int Y_setup_factory_async2(Yfactory *factory, const YCHAR *config_file);

    YAPI void Y_delete_factory(Yfactory *factory);

#ifdef __cplusplus
}
#endif

#endif
