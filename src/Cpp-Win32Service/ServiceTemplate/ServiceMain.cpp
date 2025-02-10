#include "pch.h"

SERVICE_STATUS          SvcStatus = {0};
SERVICE_STATUS_HANDLE   SvcStatusHandle = NULL;
HANDLE                  SvcStopEvent = NULL;
DWORD                   SvcCheckPoint = 1;
HANDLE                  EventSource = NULL;
HPOWERNOTIFY            PowerNotifyHandle = NULL;
HDEVNOTIFY              DeviceNotifyHandle = NULL;
HDEVNOTIFY              DeviceCustomEventHandle = NULL;
BOOL                    SessionNotify = FALSE;

VOID ReportCurrentSvcStatus()
{
    ReportSvcStatus(SvcStatus.dwCurrentState,
                    SvcStatus.dwWin32ExitCode,
                    SvcStatus.dwServiceSpecificExitCode);
}
VOID ReportSvcStatus(
    DWORD current_state,
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
    SvcStatusHandle = RegisterServiceCtrlHandlerEx(
        SVCNAME,
        SvcCtrlHandlerEx,
        NULL);

    if (NULL == SvcStatusHandle)
        return FALSE;

    SvcStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | 
                                    SERVICE_ACCEPT_STOP | 
                                    SERVICE_ACCEPT_SESSIONCHANGE | 
                                    SERVICE_ACCEPT_PRESHUTDOWN |    //system broadcast this event before shutdown.
                                    SERVICE_ACCEPT_NETBINDCHANGE |  //NIC add/remove/enable/disable
                                    SERVICE_ACCEPT_POWEREVENT;      //power event, e.g. ModernStandby, hibernate...etc.
    
    // Create an event. The control handler function, SvcCtrlHandler,
    // signals this event when it receives the stop control code.
    SvcStopEvent = CreateEvent(
        NULL,    // default security attributes
        TRUE,    // manual reset event
        FALSE,   // not signaled
        NULL);   // no name

    if (SvcStopEvent == NULL)
        return FALSE;

    //register power events to retrieve SERVICE_CONTROL_POWEREVENT in SvcCtrlHandlerEx()
    PowerNotifyHandle = RegisterModernStandbyPowerEvents(SvcStatusHandle);
    if (NULL == PowerNotifyHandle)
    {
        //write debuglog...
    }

    //register device events to monitor device changing(e.g. AddDevice, RemoveDevice...)
    //If event happen, SERVICE_CONTROL_DEVICEEVENT will be sent to SvcCtrlHandlerEx()
    //DBT_DEVTYP_DEVICEINTERFACE => monitoring device arrive/remove events
    //DBT_DEVTYP_HANDLE => can retrieve customized event from driver.
    //                     driver can send send custom events 
    //                     by IoReportTargetDeviceChange() or IoReportTargetDeviceChangeAsynchronous().
    DeviceNotifyHandle = RegisterDeviceChangeEvents(
                            SvcStatusHandle, 
                            GUID_DEVINTERFACE_DISK);
    if(NULL == DeviceNotifyHandle)
    {
        //write debuglog...
    }

#if 0
//you can also open handle to device interface of specified device...
    HANDLE handle_to_device = CreateFile("\\\\Device\\MyDevice");
    DeviceCustomEventHandle = RegisterDeviceCustomEvents(
                                    SvcStatusHandle, 
                                    GUID_MY_CUSTOM_EVENT,
                                    handle_to_device);
    if (NULL == DeviceCustomEventHandle)
    {
        //write debuglog...
    }
#endif

    //register session change events to retrieve SERVICE_CONTROL_SESSIONCHANGE in SvcCtrlHandlerEx()
    SessionNotify = RegisterSessionChangeEvents(SvcStatusHandle);
    if(!SessionNotify)
    {
        //write debuglog...
    }

    return TRUE;
}

void WINAPI ShutdownService()
{
    if(SessionNotify)
    {
        UnregisterSessionChangeEvents(SvcStatusHandle);
        SessionNotify = FALSE;
    }
    
    if (NULL != DeviceCustomEventHandle)
    {
        UnregisterDeviceChangeEvents(DeviceCustomEventHandle);
        DeviceCustomEventHandle = NULL;
    }

    if(NULL != DeviceNotifyHandle)
    {
        UnregisterDeviceChangeEvents(DeviceNotifyHandle);
        DeviceNotifyHandle = NULL;
    }

    if(NULL != PowerNotifyHandle)
    {
        UnregisterPowerSettingNotification(PowerNotifyHandle);
        PowerNotifyHandle = NULL;
    }

    if(NULL != SvcStopEvent)
    {
        CloseHandle(SvcStopEvent);
        SvcStopEvent = NULL;
    }
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR argv[])
{
    //[Recommendation]    
    // Update service status only in ServiceMain, not in EventHandler or other function.
    // It make logic lines concerntrated.

    // Report initial status to the SCM
    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 0);

    // Perform service-specific initialization and work.
    if(!InitService())
    {
        ShutdownService();
        ReportSvcStatus(SERVICE_STOPPED, EXIT_INIT_FAILED, 0);
        return;
    }

    //[Recommendation] create another thread to run your job.
    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
    while(WaitForSingleObject(SvcStopEvent, INFINITE) != WAIT_OBJECT_0)
    { 
        //do anything you want...
    }
    ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

    //TODO: Stop the thread here if you created.
    ShutdownService();

    ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

DWORD WINAPI SvcCtrlHandlerEx(
    DWORD ctrl_code,
    DWORD event_type,
    LPVOID event_data,
    LPVOID context)
{
    // Handle the requested control code. 
    // ctrl_code => translated from WM_XXXX message.
    // event_type => translated from WPARAM of WM_XXXX message.
    // event_data => translated from LPARAM of WM_XXXX message.
    // context => assigned in RegisterServiceCtrlHandlerEx()
    //[Note]
    //Actually, svchost handles WM_XXX (e.g. WM_WTSSESSION_CHANGE) message and translate it 
    // to ServiceControlHandler events. 
    //Windows message WM_XXX always comes with LPARAM and WPARAM.
    // svchost translate WPARAM to "event_type" arg, and LPARAM to "event_data" argument.
    // that's why you have these two arguments in SvcCtrlHandlerEx (HANDLER_EX type)
    // Most of WM_XXX message can be handled in service also. If you can't find reference
    // description on MSDN for them, just lookup it in Windows Message (WM_XXX) section.
    UNREFERENCED_PARAMETER(context);

    DWORD ret = NO_ERROR;
    switch (ctrl_code)
    {
    case SERVICE_CONTROL_STOP:
        ret = HandleControlStop(event_type, event_data);
        break;
    case SERVICE_CONTROL_DEVICEEVENT:   //Device changed(e.g. pluged in, removed...etc)
        ret = HandleControlDeviceEvent(event_type, event_data);
        break;
    case SERVICE_CONTROL_INTERROGATE:
        ret = HandleControlInterrogate(event_type, event_data);
        break;
    case SERVICE_CONTROL_PRESHUTDOWN:
        ret = HandleControlPreshutdown(event_type, event_data);
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        ret = HandleControlShutdown(event_type, event_data);
        break;
#if 0
//obsoleted and instead by Pnp functions.
    case SERVICE_CONTROL_NETBINDADD:
        ret = HandleControlNetBindAdd(event_type, event_data);
        break;
    case SERVICE_CONTROL_NETBINDREMOVE:
        ret = HandleControlNetBindRemove(event_type, event_data);
        break;
    case SERVICE_CONTROL_NETBINDENABLE:
        ret = HandleControlNetBindEnable(event_type, event_data);
        break;
    case SERVICE_CONTROL_NETBINDDISABLE:
        ret = HandleControlNetBindDisable(event_type, event_data);
        break;
#endif
    case SERVICE_CONTROL_POWEREVENT:
        ret = HandleControlPowerEvent(event_type, event_data);
        break;
    case SERVICE_CONTROL_SESSIONCHANGE:
        ret = HandleSessionChangeEvent(event_type, event_data);
        break;
    default:
        break;
    }
    return ret;
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
