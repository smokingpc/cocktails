#include "pch.h"

extern SERVICE_STATUS          SvcStatus;
extern SERVICE_STATUS_HANDLE   SvcStatusHandle;
extern HANDLE                  SvcStopEvent;
extern DWORD                   SvcCheckPoint;
extern HANDLE                  EventSource;

DWORD HandleControlStop(
    DWORD event_type,
    LPVOID event_data)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    // Signal the service to stop.
    SetEvent(SvcStopEvent);
    return NO_ERROR;
}
DWORD HandleControlInterrogate(
    DWORD event_type,
    LPVOID event_data)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    
//SCM use this control code to query current state and current accepted control codes.
//we should call SetServiceStatus() in this handler.
    ReportCurrentSvcStatus();
    return NO_ERROR;
}
DWORD HandleControlPreshutdown(
    DWORD event_type,
    LPVOID event_data)
{
    UNREFERENCED_PARAMETER(event_type);

    //System is going to shutdown. This event is sent from SCM.
    //If info->dwPreshutdownTimeout expired, SCM will force to kill your 
    //service process and continue shutdown progress.
    //to extend this value, use ChangeServiceConfig2() to set this value or 
    //modify registry directly.
    //You must use PreShutdown carefully. It will block entire shutdown progress until timeout.
    LPSERVICE_PRESHUTDOWN_INFO info = (LPSERVICE_PRESHUTDOWN_INFO) event_data;
    DWORD timeout = info->dwPreshutdownTimeout;//in milliseconds, default value is (10*1000)
    UNREFERENCED_PARAMETER(info);
    UNREFERENCED_PARAMETER(timeout);

    SetEvent(SvcStopEvent);
    return NO_ERROR;
}
DWORD HandleControlShutdown(
    DWORD event_type,
    LPVOID event_data)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);

    //PreShutdown phase passed. System is shutting down.
    //If service accepted SERVICE_CONTROL_PRESHUTDOWN, this event won't sent to this servce.
    SetEvent(SvcStopEvent);
    return NO_ERROR;
}

#if 0
//obsoleted and instead by Pnp functions.
DWORD HandleControlNetBindAdd(
    DWORD event_type,
    LPVOID event_data)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    UNREFERENCED_PARAMETER(context);
    return NO_ERROR;
}
DWORD HandleControlNetBindRemove(
    DWORD event_type,
    LPVOID event_data)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    UNREFERENCED_PARAMETER(context);
    return NO_ERROR;
}
DWORD HandleControlNetBindEnable(
    DWORD event_type,
    LPVOID event_data)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    UNREFERENCED_PARAMETER(context);
    return NO_ERROR;
}
DWORD HandleControlNetBindDisable(
    DWORD event_type,
    LPVOID event_data)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    UNREFERENCED_PARAMETER(context);
    return NO_ERROR;
}
#endif
