/************************************************************************
Copyright (c) 2017 MXWXZ
************************************************************************/
#include "stdafx.h"
#include "DuiMini.h"
#include "UIRecLog.h"

namespace DuiMini {
/************************************************************************
��̬��Ա��ʼ��
************************************************************************/
Reclevel UIRecLog::reclevel_ = kReclevel_Debug;
UStr UIRecLog::logfilepath_ = UStr(UISystem::GetCurrentDir())
                                + _T("\\logfile.txt");

/************************************************************************
��¼��־���Զ��壩
LPCTSTR text����¼��Ϣ
const int isexit���Ǹ�ֵ���Դ�Ϊ�˳�ֵ�˳����򣬷����˳� Ĭ��-1
����ֵ��false �ļ�����ʧ�� true ��¼�ɹ�
ע����������ĩβ��ӻس�
************************************************************************/
bool UIRecLog::RecordLog(LPCTSTR text, const int code /*= -1*/) {
    if (logfilepath_.IsEmpty())
        return false;

    FILE* file;
    _tfopen_s(&file, logfilepath_, _T("a+"));
    if (!file)
        return false;
    _ftprintf(file, text);
    fclose(file);

    if (code >= 0)
        UISystem::Exit(code);
    return true;
}

/************************************************************************
��¼��־������ʽ��
Loglevel level����־����
                kLoglevel_InfoΪ��Ϣ
                kLoglevel_WarningΪ����
                kLoglevel_ErrorΪ����
LPCTSTR text����¼��Ϣ
const int isexit���Ǹ�ֵ���Դ�Ϊ�˳�ֵ�˳����򣬷����˳� Ĭ��-1
����ֵ��false �ļ�����ʧ�� true ��¼�ɹ�
ע���������Զ���ĩβ��ӻس�
************************************************************************/
bool UIRecLog::RecordLog(Loglevel level, LPCTSTR text,
                         const int code /*= -1*/) {
    if (logfilepath_.IsEmpty())
        return false;
    if (level < reclevel_)
        return true;

    UStr strprefix;
    switch (level) {
        case kLoglevel_Info:
            strprefix = _T("[Info]");
            break;
        case kLoglevel_Warning:
            strprefix = _T("[Warning]");
            break;
        case kLoglevel_Error:
            strprefix = _T("[Error]");
            break;
    }
    time_t tm;
    time(&tm);
    TCHAR strtmp[64];
    _tcsftime(strtmp, sizeof(strtmp),
              _T("%Y-%m-%d %H:%M:%S: "), localtime(&tm));
    strprefix += strtmp;
    FILE* file;
    _tfopen_s(&file, logfilepath_, _T("a+"));
    if (!file)
        return false;
    _ftprintf(file, strprefix);
    _ftprintf(file, text);
    _ftprintf(file, _T("\n"));
    fclose(file);
    if (code >= 0)
        UISystem::Exit(code);
    return true;
}

}   // namespace DuiMini
