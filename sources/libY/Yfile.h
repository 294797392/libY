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
#include <stdint.h>

#include "Y.h"

#ifdef __cplusplus
extern "C" {
#endif

	// �洢�ļ���Ϣ
	typedef struct Yfstat_s
	{
		int exist;
		uint64_t length;
	}Yfstat;

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
	YAPI int Y_file_stat(const YCHAR *file_path, Yfstat *stat);


	/*
	 * ������
	 * ��ȡһ���ļ���������ֽ�
	 *
	 * ������
	 * @file_path��Ҫ��ȡ���ļ�������·��
	 * @bytes���ֽڻ�����
	 *
	 * ����ֵ��
	 * �ļ����ݵĳ���
	 */
	YAPI int Y_file_readbytes(const YCHAR *file_path, YBYTE **bytes, uint64_t *size);

	/*
	 * ������
	 * �ͷ�ʹ��Y_file_readall�������ٵ��ڴ�ռ�
	 *
	 * ������
	 * @content��Ҫ�ͷŵ��ڴ�ռ�
	 */
	YAPI void Y_file_free(char *content);

#ifdef __cplusplus
}
#endif

#endif