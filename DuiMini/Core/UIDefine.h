/************************************************************************
Copyright (c) 2017 MXWXZ
�ⶨ���ļ�
************************************************************************/
#pragma once
#ifndef E__CODE_DUIMINI_DUIMINI_CORE_UIDEFINE_H_
#define E__CODE_DUIMINI_DUIMINI_CORE_UIDEFINE_H_

// LIB�����������������벻Ҫ����UILIB_EXPORT����̬���������UILIB_STATIC��
#ifdef UILIB_STATIC
#   define DUIMINI_API
#else
#   if defined(UILIB_EXPORT)
#       define DUIMINI_API __declspec(dllexport)
#   else
#       define DUIMINI_API __declspec(dllimport)
#   endif
#endif

#define DUIMINI_VERSION _T("1.0")           // UI��汾

const int kMax_String_Length = MAX_PATH;    // ����ַ�������
const int kMax_XML_Attributes = 64;         // XML������Ը���

#define RESOURCE_KIND _T("ZIP")             // �Զ�����Դ����

#define EXITCODE_SUCCESS 0
#define EXITCODE_FILEFAIL 1

typedef unzFile ZFile;

#endif  // E__CODE_DUIMINI_DUIMINI_CORE_UIDEFINE_H_
