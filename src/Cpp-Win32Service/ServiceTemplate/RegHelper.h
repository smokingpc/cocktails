#pragma once
class CRegHelper {
public:
    CRegHelper() {}
    ~CRegHelper()
    {
        this->Close();
    }

    CRegHelper(const CRegHelper&) = delete; //don't allow copy
    CRegHelper(HKEY handle) { KeyHandle = handle; }

    CRegHelper& operator=(const CRegHelper&) = delete; //don't allow copy
    inline CRegHelper& operator=(HKEY& handle)
    {
        this->Close();
        this->KeyHandle = handle;
        return *this;
    }
    inline operator HKEY() const { return KeyHandle; }
    inline bool IsValid()
    {
        return (NULL != KeyHandle);
    }

    DWORD Open(HKEY base, LPCTSTR subkey, DWORD options, DWORD desired);
    inline DWORD OpenHKLM(LPCTSTR subkey, DWORD options, bool for_read)
    {
        DWORD desired = (for_read) ? KEY_READ : KEY_WRITE;
        return Open(HKEY_LOCAL_MACHINE, subkey, options, desired);
    }
    inline DWORD OpenCurrentUser(LPCTSTR subkey, DWORD options, bool for_read)
    {
        DWORD desired = (for_read) ? KEY_READ : KEY_WRITE;
        return Open(HKEY_CURRENT_USER, subkey, options, desired);
    }

    BOOL ReadValue(TCHAR *name, DWORD &buffer);
    BOOL ReadValue(TCHAR* name, TCHAR * buffer, DWORD buffer_ccb);    //buffer_ccb is size in bytes
    BOOL WriteValue(TCHAR* name, DWORD buffer);
    BOOL WriteValue(TCHAR* name, TCHAR* buffer, DWORD buffer_ccb);    //buffer_ccb is size in bytes

    void Close();
    HKEY Reset();
    void Borrow(CRegHelper& old);
protected:
    HKEY KeyHandle = NULL;

    DWORD DoRead(TCHAR* name, DWORD flag, DWORD* type, PVOID buffer, DWORD buffer_ccb);
    DWORD DoWrite(TCHAR* name, DWORD type, PVOID buffer, DWORD buffer_ccb);
};
