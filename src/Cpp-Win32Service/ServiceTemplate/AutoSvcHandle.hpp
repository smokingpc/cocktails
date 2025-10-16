#pragma once
#include <Windows.h>

class CAutoSvcHandle {
public:
    CAutoSvcHandle() {}
    CAutoSvcHandle(const CAutoSvcHandle&) = delete; //don't allow copy
    CAutoSvcHandle(SC_HANDLE handle) { SvcHandle = handle; }

    CAutoSvcHandle& operator=(SC_HANDLE& handle)
    {
        if (!this->IsNull())
            CloseServiceHandle(SvcHandle);
        this->SvcHandle = handle;
        return *this;
    }
    CAutoSvcHandle& operator=(const CAutoSvcHandle&) = delete; //don't allow copy
    operator SC_HANDLE() const { return SvcHandle; }
    inline bool IsNull()
    {
        return (NULL == SvcHandle);
    }
    ~CAutoSvcHandle()
    {
        if (!this->IsNull())
        {
            CloseServiceHandle(SvcHandle);
            SvcHandle = NULL;
        }
    }
protected:
    SC_HANDLE SvcHandle = NULL;
};
