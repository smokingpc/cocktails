#include "pch.h"

extern SERVICE_STATUS          SvcStatus;
extern SERVICE_STATUS_HANDLE   SvcStatusHandle;
extern HANDLE                  SvcStopEvent;
extern DWORD                   SvcCheckPoint;
extern HANDLE                  EventSource;

DWORD HandleControlStop(
    DWORD event_type,
    LPVOID event_data,
    LPVOID context)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    UNREFERENCED_PARAMETER(context);
    // Signal the service to stop.
    SetEvent(SvcStopEvent);
    return NO_ERROR;
}

DWORD HandleControlDeviceEvent(
    DWORD event_type,
    LPVOID event_data,
    LPVOID context)
{
    DWORD ret = NO_ERROR;
    UNREFERENCED_PARAMETER(context);

    PDEV_BROADCAST_HDR hdr = (PDEV_BROADCAST_HDR) event_data;
    if((DBT_DEVTYP_HANDLE == hdr->dbch_devicetype) && (DBT_CUSTOMEVENT == event_type))
        ret = HandleDeviceCustomEvent(event_type, hdr);
    else if (DBT_DEVTYP_DEVICEINTERFACE == hdr->dbch_devicetype)
        ret = HandleDeviceInterfaceEvent(event_type, hdr);

    return ret;
}
DWORD HandleControlInterrogate(
    DWORD event_type,
    LPVOID event_data,
    LPVOID context)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    UNREFERENCED_PARAMETER(context);
    
//SCM use this control code to query current state and current accepted control codes.
//we should call SetServiceStatus() in this handler.
    ReportCurrentSvcStatus();
    return NO_ERROR;
}
DWORD HandleControlPreshutdown(
    DWORD event_type,
    LPVOID event_data,
    LPVOID context)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(context);

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
    LPVOID event_data,
    LPVOID context)
{
    //PreShutdown phase passed. System is shutting down.
    //If service accepted SERVICE_CONTROL_PRESHUTDOWN, this event won't sent to this servce.
    SetEvent(SvcStopEvent);
    return NO_ERROR;
}
DWORD HandleControlNetBindAdd(
    DWORD event_type,
    LPVOID event_data,
    LPVOID context)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    UNREFERENCED_PARAMETER(context);
    return NO_ERROR;
}
DWORD HandleControlNetBindRemove(
    DWORD event_type,
    LPVOID event_data,
    LPVOID context)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    UNREFERENCED_PARAMETER(context);
    return NO_ERROR;
}
DWORD HandleControlNetBindEnable(
    DWORD event_type,
    LPVOID event_data,
    LPVOID context)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    UNREFERENCED_PARAMETER(context);
    return NO_ERROR;
}
DWORD HandleControlNetBindDisable(
    DWORD event_type,
    LPVOID event_data,
    LPVOID context)
{
    UNREFERENCED_PARAMETER(event_type);
    UNREFERENCED_PARAMETER(event_data);
    UNREFERENCED_PARAMETER(context);
    return NO_ERROR;
}
DWORD HandleControlPowerEvent(
    DWORD event_type,
    LPVOID event_data,
    LPVOID context)
{
    DWORD ret = NO_ERROR;
    switch(event_type)
    {
    case PBT_APMRESUMEAUTOMATIC:    
        //resume without any user activities. e.g. magic packet or BIOS wakeup event.
        //This event is also "pre-event" of PBT_APMRESUMESUSPEND
        break;
    case PBT_APMRESUMESUSPEND:
        //resumed by user activities.
        break;
#if 0
    case PBT_APMQUERYSUSPEND:
        //https://learn.microsoft.com/en-us/windows/win32/power/pbt-apmquerysuspend
        //this event removed since VISTA.
        break;
#endif
    case PBT_APMSUSPEND:
        //computer is entering suspend mode.
        break;
    case PBT_APMPOWERSTATUSCHANGE:  
        //power source AC to DC, or DC to AC event.
        //this event also sent when DC power(battery) is lower than "low power threshold".
        break;
    case PBT_POWERSETTINGCHANGE:
        break;
    }

    return ret;
}
DWORD HandleSessionChangeEvent(
    DWORD event_type,
    LPVOID event_data,
    LPVOID context)
{
    return NO_ERROR;
}
