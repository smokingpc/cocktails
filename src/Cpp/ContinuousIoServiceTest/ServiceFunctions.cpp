#include "CommonHeader.h"

HANDLE TargetFile = INVALID_HANDLE_VALUE;
SERVICE_STATUS_HANDLE StatusHandle = NULL;
HANDLE EventSrc = INVALID_HANDLE_VALUE;
DWORD SvcCheckPoint = 1;

bool InitService()
{
    EventSrc = RegisterEventSource(NULL, SERVICE_NAME);
    if (NULL == EventSrc || INVALID_HANDLE_VALUE == EventSrc)
        return false;

    StatusHandle = RegisterServiceCtrlHandlerEx(SERVICE_NAME, (LPHANDLER_FUNCTION_EX)SvcCtrlHandler, 0);
    if (NULL == StatusHandle)
    {
        ReportSvcEvent((LPTSTR)_T("RegisterServiceCtrlHandlerEx failed."));
        return false;
    }
}

void ShutdownService()
{
    if(INVALID_HANDLE_VALUE != EventSrc && NULL != EventSrc)
        DeregisterEventSource(EventSrc);
}


VOID WINAPI SvcCtrlHandler(DWORD ctrl_code, DWORD type, LPVOID data, LPVOID ctx)
{
    switch (ctrl_code)
    {
    case SERVICE_CONTROL_PRESHUTDOWN:
    case SERVICE_CONTROL_STOP:
        break;
    case SERVICE_CONTROL_INTERROGATE:
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
        .dwCurrentState = state,
        .dwWin32ExitCode = exit_code,
        .dwWaitHint = wait_hint,
    };

    switch (state) {
    case SERVICE_STOP_PENDING:
    case SERVICE_START_PENDING:
        status.dwControlsAccepted = 0;
        status.dwCheckPoint = SvcCheckPoint++;
        break;
    case SERVICE_RUNNING:
        status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PRESHUTDOWN;
        status.dwCheckPoint = 0;
        break;
    case SERVICE_STOPPED:
        status.dwControlsAccepted = 0;
        status.dwCheckPoint = 0;
        break;
    default:
        status.dwControlsAccepted = 0;
        status.dwCheckPoint = SvcCheckPoint++;
    }

    // Report the status of the service to the SCM.
    SetServiceStatus(StatusHandle, &status);
}


VOID ReportSvcEvent(DWORD event_id, LPTSTR msg)
{
//https://learn.microsoft.com/en-us/windows/win32/eventlog/reporting-an-event
    HANDLE hEventSource;
    LPCTSTR msg_array[2] = {NULL};
    //TCHAR buffer[256] = {0};

    if (NULL != hEventSource)
    {
        //StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

        msg_array[0] = SERVICE_NAME;
        msg_array[1] = msg;

        ReportEvent(hEventSource,        // event log handle
            EVENTLOG_ERROR_TYPE, // event type
            0,                   // event category
            event_id,           // event identifier
            NULL,                // no security identifier
            2,                   // size of lpszStrings array
            0,                   // no binary data
            msg_array,         // array of strings
            NULL);               // no binary data

        DeregisterEventSource(hEventSource);
    }
}

void ServiceMain(DWORD argc, LPTSTR *argv)
{
    LPTSTR filepath = argv[0];
    if(!InitService())
        goto END;


END:
    ShutdownService();
}