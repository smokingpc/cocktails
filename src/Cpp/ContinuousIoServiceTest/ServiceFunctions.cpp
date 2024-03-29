#include "CommonHeader.h"

HANDLE TargetFile = INVALID_HANDLE_VALUE;
SERVICE_STATUS_HANDLE StatusHandle = NULL;
HANDLE EventSrc = INVALID_HANDLE_VALUE;
DWORD SvcCheckPoint = 1;
HANDLE EventStop = INVALID_HANDLE_VALUE;
DWORD SvcState = SERVICE_STOPPED;

VOID WINAPI SvcCtrlHandler(DWORD ctrl_code, DWORD type, LPVOID data, LPVOID ctx)
{
//hint means "how long should SCM wait my service changing the state?"
    DWORD hint = 0;
    switch (ctrl_code)
    {
    case SERVICE_CONTROL_PRESHUTDOWN:
        DebugBreak();
    case SERVICE_CONTROL_STOP:
        ReportSvcStatus(SERVICE_STOP_PENDING, 0, SERVICE_WAITHINT);
        SetEvent(EventStop);
        break;
    case SERVICE_CONTROL_INTERROGATE:
        if(SERVICE_START_PENDING == SvcState || SERVICE_STOP_PENDING == SvcState)
            hint = SERVICE_WAITHINT;
        ReportSvcStatus(SvcState, 0, hint);
        break;
    default:
        break;
    }
}

VOID ReportSvcStatus(DWORD state, DWORD exit_code, DWORD wait_hint)
{
    if (NULL == StatusHandle)
        return;

    SERVICE_STATUS status = 
    {
        .dwServiceType = SERVICE_WIN32_OWN_PROCESS,
        .dwCurrentState = state,
        .dwWin32ExitCode = exit_code,
        .dwWaitHint = wait_hint,
    };
    SvcState = state;
    switch (state) {
    case SERVICE_STOP_PENDING:
    case SERVICE_START_PENDING:
        status.dwControlsAccepted = 0;
        status.dwCheckPoint = ++SvcCheckPoint;
        break;
    case SERVICE_RUNNING:
        SvcCheckPoint = 0;
        status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PRESHUTDOWN;
        status.dwCheckPoint = 0;
        break;
    case SERVICE_STOPPED:
        SvcCheckPoint = 0;
        status.dwControlsAccepted = 0;
        status.dwCheckPoint = 0;
        break;
    default:
        status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PRESHUTDOWN;
        status.dwCheckPoint = ++SvcCheckPoint;
    }

    // Report the status of the service to the SCM.
    SetServiceStatus(StatusHandle, &status);
}

VOID ReportSvcEvent(WORD event_type, DWORD event_id, LPTSTR msg)
{
    //https://learn.microsoft.com/en-us/windows/win32/eventlog/reporting-an-event
    LPCTSTR msg_array[2] = { NULL };

    if (NULL == EventSrc)
        return;

    msg_array[0] = SERVICE_NAME;
    msg_array[1] = msg;

    ReportEvent(EventSrc,       // event log handle
        event_type,    // event type
        0,                      // event category
        event_id,               // event identifier
        NULL,                   // no security identifier
        _countof(msg_array),    // how many elements in message array?
        0,                      // no binary data
        msg_array,              // array of strings
        NULL);                  // no binary data
}

void DoContinuousIo(LPTSTR filepath)
{
    HANDLE file = CreateFile(filepath, GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
    BYTE *buffer = (BYTE*)IO_DATA;
    BOOL write_ok = FALSE;
    DWORD written = 0;
    DWORD error = 0;
    if(INVALID_HANDLE_VALUE == file)
        return;

    while(WAIT_OBJECT_0 != WaitForSingleObject(EventStop, IO_INTERVAL))
    {
        //do I/O
        write_ok = WriteFile(file, buffer, IO_SIZE, &written, NULL);
        if(!write_ok)
        {
            error = GetLastError();
            ReportSvcErrorEvent((LPTSTR)_T("DoContinuousIo failed in event loop."));
//            DebugBreak();
            break;
        }
        FlushFileBuffers(file);
    }
    
    write_ok = WriteFile(file, buffer, IO_SIZE, &written, NULL);
//    DebugBreak();
    if (!write_ok)
    {
        ReportSvcErrorEvent((LPTSTR)_T("DoContinuousIo failed after loop exit."));
        error = GetLastError();
//        DebugBreak();
    }
    CloseHandle(file);
}

bool InitService()
{
    EventSrc = RegisterEventSource(NULL, SERVICE_NAME);
    if (NULL == EventSrc || INVALID_HANDLE_VALUE == EventSrc)
        return false;

    StatusHandle = RegisterServiceCtrlHandlerEx(SERVICE_NAME, (LPHANDLER_FUNCTION_EX)SvcCtrlHandler, 0);
    if (NULL == StatusHandle)
    {
        ReportSvcErrorEvent((LPTSTR)_T("RegisterServiceCtrlHandlerEx failed."));
        return false;
    }

    EventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == EventStop || INVALID_HANDLE_VALUE == EventStop)
        return false;

    CheckAndSetShutdownParams();
    return true;
}
void ShutdownService()
{
    if (INVALID_HANDLE_VALUE != EventSrc && NULL != EventSrc)
        DeregisterEventSource(EventSrc);
}
void ServiceMain(DWORD argc, LPTSTR *argv)
{
    LPTSTR filepath = argv[0];
    if(argc < 2)
        filepath = DEFAULT_IO_TARGET;

    ReportSvcStatus(SERVICE_START_PENDING, 0, SERVICE_WAITHINT);

    if(!InitService())
        goto END;

    ReportSvcStatus(SERVICE_RUNNING, 0, 0);
    ReportSvcInfoEvent((LPTSTR)_T("SERVICE_RUNNING!"));
    DoContinuousIo(filepath);

END:
    ShutdownService();
    ReportSvcStatus(SERVICE_STOPPED, 0, 0);
}