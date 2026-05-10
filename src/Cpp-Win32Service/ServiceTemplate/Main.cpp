#include "pch.h"
#pragma comment(lib, "advapi32.lib")

using namespace std;
VOID SvcInstall(bool auto_restart)
{
    TCHAR exe_path[MAX_PATH] = {0};

    if (!GetModuleFileName(NULL, exe_path, MAX_PATH))
    {
        _tprintf(_T("Cannot install service (%d)\n)"), GetLastError());
        return;
    }

    // In case the path contains a space, it must be quoted so that
    // it is correctly interpreted. For example,
    // "d:\my share\myservice.exe" should be specified as
    // ""d:\my share\myservice.exe"".
    // **This is because a file path need double quote to escape space in path string
    // e.g. c:\program files\my service\service.exe  => we should double quote it to "c:\program files\my service\service.exe"
    TCHAR svc_path[MAX_PATH] = {0};
    StringCbPrintf(svc_path, MAX_PATH, TEXT("\"%s\""), exe_path);

    // Get a handle to the SCM database. 

    CAutoSvcHandle scm = OpenSCManagerW(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == scm)
    {
        _tprintf(_T("OpenSCManager failed (%d)\n"), GetLastError());
        return;
    }

    // Create the service
    CAutoSvcHandle service = CreateService(
        scm,              // SCM database 
        SVCNAME,                   // name of service 
        SVCNAME,                   // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        svc_path,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 

    if (service == NULL)
        _tprintf(_T("CreateService failed (%d)\n"), GetLastError());
    else 
        _tprintf(_T("Service installed successfully\n"));

    //noncrash_restart : if service exit with Win32ErrorCode != 0, following steps
    //can make SCM restart this service automatically even service is not crashed.
    //In original FailureActions, SCM only restart service which is crashed.
    if (auto_restart)
    {
        //setup FailureActions. In "Recovery" tab of "Service Management" mmc, you can see
        //3 Failure Actions settings.
        unique_ptr<BYTE> ptr(new BYTE[LARGE_BUFFER_SIZE]);
        memset(ptr.get(), 0, LARGE_BUFFER_SIZE);
        SERVICE_FAILURE_ACTIONS *actions = (SERVICE_FAILURE_ACTIONS*)ptr.get();
        actions->lpsaActions = (SC_ACTION*)(ptr.get() + sizeof(SERVICE_FAILURE_ACTIONS));
        actions->cActions = MAX_FAILURE_ACTIONS;
        for (int i = 0; i < MAX_FAILURE_ACTIONS; i++) {
            actions->lpsaActions[i].Type = SC_ACTION_RESTART;
            actions->lpsaActions[i].Delay = FAILURE_ACTION_DELAY;
        }
        ChangeServiceConfig2(service, SERVICE_CONFIG_FAILURE_ACTIONS, actions);
    }
}
VOID SvcUninstall() {
    CAutoSvcHandle scm = OpenSCManagerW(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == scm)
    {
        _tprintf(_T("OpenSCManager failed (%d)\n"), GetLastError());
        return;
    }

    CAutoSvcHandle service = OpenService(scm, SVCNAME, SERVICE_STOP | DELETE);
    if (!service.IsValid()) 
    {
        SERVICE_STATUS status = {0};
        _tprintf(_T("OpenService failed (%d)\n"), GetLastError());
        return;
    }

    SERVICE_STATUS status = {0};
    if (!ControlService(service, SERVICE_CONTROL_STOP, &status)) 
    {
        _tprintf(_T("Failed to stop service\n"));
    }

    if (!DeleteService(service)) 
    {
        _tprintf(_T("DeleteService failed (%d)\n"), GetLastError());
    }
}

//   Entry point of EXE
int _tmain(int argc, TCHAR* argv[])
{
    // If command-line parameter is "install", install the service. 
    // Otherwise, the service is probably being started by the SCM.
    if (lstrcmpi(argv[1], TEXT("-install")) == 0)
    {
        SvcInstall();
        return 0;
    }
    else if (lstrcmpi(argv[1], TEXT("-remove")) == 0) 
    {
        SvcUninstall();
        return 0;
    }

    // ServiceMain is entry function of service. 
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { SVCNAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };
    // This call returns when the service has stopped. 
    // The process should simply terminate when the call returns.
    if (!StartServiceCtrlDispatcher(DispatchTable)) {
    //todo: write error log
        return ERROR_FAILED_SERVICE_CONTROLLER_CONNECT;
    }

    //don't do anything after StartServiceCtrlDispatcher() returned!!
    return ERROR_SUCCESS;
}
