// QueryServiceStatusTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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
    _tprintf(_T("Start Query\n"));
    if (argc < 4)
    {
        _tprintf(_T("Usage:  QueryServiceStatusTest.exe [service name]\n"));
        return -1;
    }
    TCHAR *svcname = argv[1];
    DWORD scm_access = 0;
    DWORD svc_access = 0;
    
    if (argc > 2)
        scm_access = _ttol(argv[2]);
    if (argc > 3)
        svc_access = _ttol(argv[3]);

    _tprintf(_T("Start Query: svc_name=%s, scm_access=%X, svc_access=%X\n"),
        svcname, scm_access, svc_access);

    //SC_MANAGER_CONNECT
    CAutoSvcHandle scm = OpenSCManager(NULL, NULL, scm_access);
    if (scm.IsNull()) {
        _tprintf(_T("OpenSCManager failed, err: %d\n"), GetLastError());
        return -1;
    }

    _tprintf(_T("scm handle = %p\n"), (SC_HANDLE)scm);

    //SERVICE_ALL_ACCESS
    CAutoSvcHandle svc = OpenService(scm, svcname, svc_access);
    if (svc.IsNull()) {
        _tprintf(_T("OpenService failed, err: %d\n"), GetLastError());
        return -1;
    }

    _tprintf(_T("svc handle = %p\n"), (SC_HANDLE)svc);

    SERVICE_STATUS_PROCESS status = { 0 };
    DWORD need_size = 0;
    if (QueryServiceStatusEx(svc, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, sizeof(status), &need_size))
    {
        _tprintf(_T("QueryServiceStatusEx\n"));

        const TCHAR *state_str = NULL;
        switch (status.dwCurrentState)
        {
        case SERVICE_STOPPED:
            state_str = _T("SERVICE_STOPPED");
            break;
        case SERVICE_START_PENDING:
            state_str = _T("SERVICE_START_PENDING");
            break;
        case SERVICE_STOP_PENDING:
            state_str = _T("SERVICE_STOP_PENDING");
            break;
        case SERVICE_RUNNING:
            state_str = _T("SERVICE_RUNNING");
            break;
        case SERVICE_CONTINUE_PENDING:
            state_str = _T("SERVICE_CONTINUE_PENDING");
            break;
        case SERVICE_PAUSE_PENDING:
            state_str = _T("SERVICE_PAUSE_PENDING");
            break;
        case SERVICE_PAUSED:
            state_str = _T("SERVICE_PAUSED");
            break;
        }
        _tprintf(_T("Service[%s] State=%s\n"), svcname, state_str);
    }
    else
    {
        _tprintf(_T("QueryServiceStatusEx failed(%d)\n"), GetLastError());
    }

    return 0;
}
