#include <tchar.h>
#include <windows.h>

class CAutoSvcHandle {
public:
    CAutoSvcHandle() {}
    CAutoSvcHandle(const CAutoSvcHandle&) = delete; //don't allow copy
    CAutoSvcHandle(SC_HANDLE handle) { SvcHandle = handle; }

    CAutoSvcHandle& operator=(const CAutoSvcHandle&) = delete; //don't allow copy
    CAutoSvcHandle& operator=(SC_HANDLE& handle)
    {
        if (!this->IsNull())
            CloseServiceHandle(SvcHandle);
        this->SvcHandle = handle;
        _tprintf(_T("got handle = %p"), handle);
        return *this;
    }

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


//   Entry point of EXE
int _tmain(int argc, TCHAR* argv[]) 
{
    if (argc < 2)
    {
        _tprintf(_T("Usage:  StartServiceTest.exe [service name] [SCM desired access] [Service desired service]\n"));
        return -1;
    }
    TCHAR* svcname = argv[1];
    DWORD scm_access = SC_MANAGER_CONNECT | SC_MANAGER_ENUMERATE_SERVICE;
    DWORD svc_access = SERVICE_START | SERVICE_QUERY_STATUS;

    if (argc > 2)
        scm_access = _ttol(argv[2]);
    if (argc > 3)
        svc_access = _ttol(argv[3]);

    _tprintf(_T("Start Query: svc_name=%s, scm_access=%X, svc_access=%X\n"),
        svcname, scm_access, svc_access);

    //SC_MANAGER_CONNECT  0x01
    CAutoSvcHandle scm = OpenSCManager(NULL, NULL, scm_access);
    if (scm.IsNull()) {
        _tprintf(_T("OpenSCManager failed, err: %d\n"), GetLastError());
        return -1;
    }

    _tprintf(_T("scm handle = %p\n"), (SC_HANDLE)scm);

    //SERVICE_START 0x10
    CAutoSvcHandle svc = OpenService(scm, svcname, svc_access);
    if (svc.IsNull()) {
        _tprintf(_T("OpenService failed, err: %d\n"), GetLastError());
        return -1;
    }

    if (!StartService(svc, 0, NULL))
    {
        _tprintf(_T("StartService failed, err: %d\n"), GetLastError());
    }

    return 0;
}
