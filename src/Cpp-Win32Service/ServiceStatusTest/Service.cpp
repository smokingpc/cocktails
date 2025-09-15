#include "CommonInclude.h"

SERVICE_STATUS          SvcStatus = { 0 };
SERVICE_STATUS_HANDLE   SvcStatusHandle = NULL;
HANDLE                  SvcStopEvent = NULL;
DWORD                   SvcCheckPoint = 1;
HANDLE                  EventSource = NULL;

void WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
    // Handle the requested control code. 
    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        //UpdateSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
        // Signal the service to stop.
        SetEvent(SvcStopEvent);
        return;

    case SERVICE_CONTROL_INTERROGATE:
        break;

    default:
        break;
    }
}
void WINAPI ServiceMain(DWORD argc, LPTSTR argv[])
{
    //in service's argument, it is assigned when install service.
    // Perform service-specific initialization and work.
    if (!InitService())
        return;

    while (WAIT_OBJECT_0 != WaitForSingleObject(SvcStopEvent, CHECK_INTERVAL))
    {

    }

    ShutdownService();
}
BOOL WINAPI InitService()
{
    // Register the handler function for the service
    SvcStatusHandle = RegisterServiceCtrlHandler(
        SVCNAME,
        SvcCtrlHandler);

    if (NULL == SvcStatusHandle)
        return FALSE;

    SvcStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;

    // Report initial status to the SCM
    UpdateSvcStatus(SERVICE_START_PENDING, NO_ERROR, 0);

    // Create an event. The control handler function, SvcCtrlHandler,
    // signals this event when it receives the stop control code.
    SvcStopEvent = CreateEvent(
        NULL,    // default security attributes
        TRUE,    // manual reset event
        FALSE,   // not signaled
        NULL);   // no name

    if (SvcStopEvent == NULL)
        return FALSE;

    UpdateSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
    return TRUE;
}
void WINAPI ShutdownService()
{
    UpdateSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
    CloseHandle(SvcStopEvent);
    UpdateSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}
void UpdateSvcStatus(
    DWORD current_state,
    DWORD win32_exit,
    DWORD svc_exit,
    DWORD wait)
{

    // Fill in the SERVICE_STATUS structure.
    SvcStatus.dwServiceType = SVCTYPE;
    SvcStatus.dwCurrentState = current_state;
    SvcStatus.dwWin32ExitCode = win32_exit;
    SvcStatus.dwServiceSpecificExitCode = svc_exit;
    SvcStatus.dwWaitHint = wait;
    
    //to make service auto restart in non-crashing case, we must setup 
    //FailureAction Flag. If not set this flag, FailureAction only works when crash.
    //ChangeServiceConfig2(handle, SERVICE_CONFIG_FAILURE_ACTIONS_FLAG, struct SERVICE_FAILURE_ACTIONS_FLAG);

    if ((current_state == SERVICE_RUNNING) ||
        (current_state == SERVICE_STOPPED))
        SvcCheckPoint = SvcStatus.dwCheckPoint = 0;
    else
        SvcStatus.dwCheckPoint = SvcCheckPoint++;

    // Report the status of the service to the SCM.
    SetServiceStatus(SvcStatusHandle, &SvcStatus);
}
