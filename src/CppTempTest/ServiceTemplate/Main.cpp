#include "pch.h"
#pragma comment(lib, "advapi32.lib")

VOID SvcInstall()
{
    SC_HANDLE scm = NULL;
    SC_HANDLE service = NULL;
    TCHAR exe_path[MAX_PATH] = {0};

    if (!GetModuleFileName(NULL, exe_path, MAX_PATH))
    {
        printf("Cannot install service (%d)\n", GetLastError());
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

    scm = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == scm)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    // Create the service
    service = CreateService(
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
    {
        printf("CreateService failed (%d)\n", GetLastError());
        CloseServiceHandle(scm);
        return;
    }
    else 
        printf("Service installed successfully\n");

    CloseServiceHandle(service);
    CloseServiceHandle(scm);
}



//   Entry point of EXE
int __cdecl _tmain(int argc, TCHAR* argv[])
{
    SetupEventReporter();

    // If command-line parameter is "install", install the service. 
    // Otherwise, the service is probably being started by the SCM.
    if (lstrcmpi(argv[1], TEXT("install")) == 0)
    {
        SvcInstall();
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
    if (!StartServiceCtrlDispatcher(DispatchTable))
        ReportEventLog(SVC_ERROR, EVTMSG("Start service failed"), GetLastError());

    TeardownEventReporter();
}
