#include <windows.h>
#include "RegHelper.h"


DWORD CRegHelper::Open(HKEY base, LPCTSTR subkey, DWORD options, DWORD desired)
{
    LSTATUS status = RegOpenKeyEx(base, subkey, options, desired, &this->KeyHandle);
    if (ERROR_SUCCESS != status) {
        this->KeyHandle = NULL;
    }
    return status;
}
BOOL CRegHelper::ReadValue(TCHAR* name, DWORD& buffer) 
{
    if (!this->IsValid()) {
        return FALSE;
    }
    if (NULL == name) {
        return FALSE;
    }
    DWORD type = 0;
    DWORD err = DoRead(name, (RRF_RT_DWORD | RRF_ZEROONFAILURE), &type, &buffer, sizeof(buffer));
    //todo: check error reason
    
    return (ERROR_SUCCESS == err);
}

BOOL CRegHelper::ReadValue(TCHAR* name, TCHAR* buffer, DWORD buffer_ccb)
{
    //buffer_ccb is size in bytes
    if (!this->IsValid()) {
        return FALSE;
    }
    if (NULL == name) {
        return FALSE;
    }
    DWORD type = 0;
    DWORD err = DoRead(name, (RRF_RT_REG_SZ | RRF_ZEROONFAILURE), &type, buffer, buffer_ccb);
    //todo: check error reason

    return (ERROR_SUCCESS == err);
}

BOOL CRegHelper::WriteValue(TCHAR* name, DWORD buffer)
{
    if (!this->IsValid()) {
        return FALSE;
    }
    if (NULL == name) {
        return FALSE;
    }
    DWORD type = REG_DWORD;
    DWORD err = DoWrite(name, type, &buffer, sizeof(buffer));
    //todo: check error reason

    return (ERROR_SUCCESS == err);
}

BOOL CRegHelper::WriteValue(TCHAR* name, TCHAR* buffer, DWORD buffer_ccb)
{
    //buffer_ccb is size in bytes
    if (!this->IsValid()) {
        return FALSE;
    }
    if (NULL == name) {
        return FALSE;
    }
    DWORD type = REG_SZ;
    DWORD err = DoWrite(name, type, buffer, buffer_ccb);
    //todo: check error reason

    return (ERROR_SUCCESS == err);
}

void CRegHelper::Close()
{
    if (!this->IsValid())
    {
        RegCloseKey(this->KeyHandle);
        KeyHandle = NULL;
    }
}
HKEY CRegHelper::Reset()
{
    HKEY ret = this->KeyHandle;
    this->KeyHandle = NULL;
    return ret;
}
void CRegHelper::Borrow(CRegHelper& old)
{
    HKEY handle = old.Reset();
    this->Close();
    this->KeyHandle = handle;
}

DWORD CRegHelper::DoRead(TCHAR* name, DWORD flag, DWORD *type, PVOID buffer, DWORD buffer_ccb)
{
    return RegGetValue(this->KeyHandle, NULL, name, flag, type, buffer, &buffer_ccb);
}

DWORD CRegHelper::DoWrite(TCHAR* name, DWORD type, PVOID buffer, DWORD buffer_ccb)
{
    return RegSetValueEx(this->KeyHandle, name, 0, type, (const BYTE*)buffer, buffer_ccb);
}
