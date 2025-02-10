#include "pch.h"
#include <shlwapi.h>
#pragma comment(lib , "Shlwapi.lib")

SERVICE_STATUS          SvcStatus = {0};
SERVICE_STATUS_HANDLE   SvcStatusHandle = NULL;
HANDLE                  SvcStopEvent = NULL;
DWORD                   SvcCheckPoint = 1;
HANDLE                  EventSource = NULL;

void WriteOutputMsg(TCHAR* filepath, TCHAR* msg)//, va_list arglist)
{
    HANDLE file = CreateFile(filepath,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (NULL != file)
    {
        DWORD written = 0;
        SetFilePointer(file, 0, NULL, FILE_END);
        WriteFile(file, msg, (DWORD)(_tcslen(msg) * sizeof(TCHAR)), &written, NULL);
        CloseHandle(file);
    }
}

//cch stand for "Count of CHaracters"
void InitOutfilePath(LPTSTR svcname, LPTSTR buffer, DWORD max_cch)
{
    TCHAR temp[1024] = { 0 };
    DWORD ret = GetModuleFileName(NULL, temp, _countof(temp));
    if (ret > 0)
    {
        PathRemoveFileSpec(temp);
        PathAppend(temp, svcname);
        PathAddExtension(temp, _T(".txt"));
        _tcscpy_s(buffer, max_cch, temp);
    }
}

static VOID ReportSvcStatus(DWORD current_state,
    DWORD win32_exit,
    DWORD svc_exit,
    DWORD wait)
{

    // Fill in the SERVICE_STATUS structure.
    SvcStatus.dwServiceType = SVCTYPE;
    SvcStatus.dwCurrentState = current_state;
    SvcStatus.dwWin32ExitCode = win32_exit;
    SvcStatus.dwWaitHint = wait;
    SvcStatus.dwServiceSpecificExitCode = svc_exit;

    if ((current_state == SERVICE_RUNNING) ||
        (current_state == SERVICE_STOPPED))
        SvcStatus.dwCheckPoint = 0;
    else 
        SvcStatus.dwCheckPoint = SvcCheckPoint++;

    // Report the status of the service to the SCM.
    SetServiceStatus(SvcStatusHandle, &SvcStatus);
}

BOOL SetupEventReporter()
{
    //register EventSource and get handle.
    //It is used to call ReportEvent(), which report to event log of system.
    EventSource = RegisterEventSource(NULL, SVCNAME);
    if (NULL == EventSource)
        return FALSE;

    return TRUE;
}
void TeardownEventReporter()
{
    DeregisterEventSource(EventSource);
    EventSource = NULL;
}

BOOL WINAPI InitService()
{
    // Register the handler function for the service
    SvcStatusHandle = RegisterServiceCtrlHandler(
        SVCNAME,
        SvcCtrlHandler);

    if (NULL == SvcStatusHandle)
        return FALSE;

    SvcStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP ;

    // Report initial status to the SCM
    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 0);

    // Create an event. The control handler function, SvcCtrlHandler,
    // signals this event when it receives the stop control code.
    SvcStopEvent = CreateEvent(
        NULL,    // default security attributes
        TRUE,    // manual reset event
        FALSE,   // not signaled
        NULL);   // no name

    if (SvcStopEvent == NULL)
        return FALSE;

    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
    return TRUE;
}

void WINAPI ShutdownService()
{
    ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR argv[])
{
    TCHAR outfile[512] = { 0 };

    // Perform service-specific initialization and work.
    if(!InitService())
    {
        ReportSvcStatus(SERVICE_STOPPED, EXIT_INIT_FAILED, 0);
        return;
    }

    //argv[0] is always SERVICE_NAME.
    //following argv[] are passed from StartService() API.
    InitOutfilePath(argv[0], outfile, _countof(outfile));

    //TODO: It is recommended to create another thread to run your job.
    UINT64 counter = 0;
    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
    while(WaitForSingleObject(SvcStopEvent, DEFAULT_SVC_WAIT) != WAIT_OBJECT_0)
    { 
        TCHAR msg[64] = {0};
        _stprintf_s(msg, _countof(msg), _T("=>%llu\n"), counter++);
        //do anything you want...
        WriteOutputMsg(outfile, msg);
    }

    //TODO: Stop the thread here if you created.
    ShutdownService();
}

VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
    // Handle the requested control code. 

    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
        // Signal the service to stop.
        SetEvent(SvcStopEvent);
        return;

    case SERVICE_CONTROL_INTERROGATE:
        break;

    default:
        break;
    }

}

VOID ReportEventLog(DWORD event_id, LPTSTR msg, DWORD last_error)
{
    LPCTSTR report[2] = {0};
    TCHAR buffer[SMALL_BUFFER_SIZE] = {0};

    StringCchPrintf(buffer, SMALL_BUFFER_SIZE, _T("%s, error = %d"), msg, last_error);

    report[0] = SVCNAME;
    report[1] = buffer;

    ReportEvent(EventSource,        // event log handle
        EVENTLOG_ERROR_TYPE, // event type
        0,                   // event category
        event_id,           // event identifier
        NULL,                // no security identifier
        2,                   // size of lpszStrings array
        0,                   // no binary data
        report,              // array of report strings
        NULL);               // no binary data

}
