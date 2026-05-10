#pragma once
#include <Windows.h>

class CAutoSvcHandle {
public:
    CAutoSvcHandle() {}
    CAutoSvcHandle(const CAutoSvcHandle&) = delete; //don't allow copy
    CAutoSvcHandle(SC_HANDLE handle) { SvcHandle = handle; }
    ~CAutoSvcHandle()
    {
        this->Close();
    }

    CAutoSvcHandle& operator=(SC_HANDLE& handle)
    {
        this->Close();
        this->SvcHandle = handle;
        return *this;
    }
    CAutoSvcHandle& operator=(const CAutoSvcHandle&) = delete; //don't allow copy
    operator SC_HANDLE() const { return SvcHandle; }
    inline bool IsValid()
    {
        return (NULL == SvcHandle);
    }
    void Close() 
    {
        if (!this->IsValid())
        {
            CloseServiceHandle(SvcHandle);
            SvcHandle = NULL;
        }
    }
    SC_HANDLE Reset() 
    {
        SC_HANDLE ret = this->SvcHandle;
        this->SvcHandle = NULL;
        return ret;
    }
    void Borrow(CAutoSvcHandle& old) 
    {
        SC_HANDLE handle = old.Reset();
        this->Close();
        this->SvcHandle = handle;
    }
protected:
    SC_HANDLE SvcHandle = NULL;
};
