/***********************************************************************************
 * @ file    : Yfile.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.09.09 06:43
 * @ brief   : ��װ�ļ����������ݿ��ַ��͵��ֽ��ַ�
 * @ remark  ��
 ************************************************************************************/

#ifndef __YFILE_H__
#define __YFILE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Y.h"

#ifdef __cplusplus
extern "C" {
#endif

	/*
	 * ������
	 * ���ֽ�Ϊ��λ�����ļ���С
	 *
	 * ������
	 * @file_path��Ҫ��ȡ���ļ�������·��
	 *
	 * ����ֵ��
	 * �ļ���С�����ֽ�Ϊ��λ
	 */
	YAPI size_t Y_file_size(const YCHAR *file_path);


    /*
	 * ������
	 * ��ȡһ���ļ������������
	 *
	 * ������
	 * @file_path��Ҫ��ȡ���ļ�������·��
	 * @content�������ļ�����
	 * 
	 * ����ֵ��
	 * �ļ����ݵĳ���
	 */
	YAPI int Y_file_readall(const YCHAR *file_path, YCHAR **content);

	/*
	 * ������
	 * �ͷ�ʹ��Y_file_readall�������ٵ��ڴ�ռ�
	 *
	 * ������
	 * @content��Ҫ�ͷŵ��ڴ�ռ�
	 */
	YAPI void Y_file_free(const YCHAR *content);

#ifdef __cplusplus
}
#endif

#endif