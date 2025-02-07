#include "pch.h"
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "PowrProf.lib")

BOOL IsModernStandbySupported()
{
    SYSTEM_POWER_CAPABILITIES cap = { 0 };
    NTSTATUS ret = CallNtPowerInformation(
        SystemPowerCapabilities,
        NULL,
        0,
        &cap,
        sizeof(cap));
    if (ret != ERROR_SUCCESS)
    {
        //write debuglog.... 
    }

    return (TRUE == cap.AoAc);
}

HPOWERNOTIFY RegisterModernStandbyPowerEvents(SERVICE_STATUS_HANDLE svc_handle)
{
    HPOWERNOTIFY handle = NULL;
    if (IsModernStandbySupported() && NULL != svc_handle)
    {
        //GUID_CONSOLE_DISPLAY_STATE is available since Win8, 
        //used for AoAc(modern standby in Win10)
        handle = RegisterPowerSettingNotification(
            svc_handle,
            &GUID_CONSOLE_DISPLAY_STATE,
            DEVICE_NOTIFY_SERVICE_HANDLE);
        if (NULL == handle)
        {
            //write debuglog...
        }
    }
    return handle;
}
void UnregisterModernStandbyPowerEvents(HPOWERNOTIFY handle)
{
    if(!UnregisterPowerSettingNotification(handle))
    {
        //write debuglog...
    }
}
HDEVNOTIFY RegisterDeviceChangeEvents(
    SERVICE_STATUS_HANDLE svc_handle, 
    const GUID& class_guid)
{
//refer to https://learn.microsoft.com/zh-tw/windows/win32/api/winuser/nf-winuser-registerdevicenotificationa
//filter type is NOT bitwise. If want to monitor multiple type of events, 
//call this function multiple times....
    DEV_BROADCAST_DEVICEINTERFACE filter = {0};
    filter.dbcc_size = sizeof(filter);
    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    filter.dbcc_classguid = class_guid; //e.g. GUID_DEVINTERFACE_DISK to monitor event of disk(inserted or ejected)

    HDEVNOTIFY handle = RegisterDeviceNotification(
                            svc_handle, 
                            &filter, 
                            DEVICE_NOTIFY_SERVICE_HANDLE);
    return handle;
}
HDEVNOTIFY RegisterDeviceCustomEvents(
    SERVICE_STATUS_HANDLE svc_handle, 
    const GUID& event_guid,
    HANDLE device_handle)
{
    //refer to https://learn.microsoft.com/zh-tw/windows/win32/api/winuser/nf-winuser-registerdevicenotificationa
    //filter type is NOT bitwise. If want to monitor multiple type of events, 
    //call this function multiple times....
    DEV_BROADCAST_HANDLE filter = { 0 };
    filter.dbch_size = sizeof(filter);
    filter.dbch_devicetype = DBT_DEVTYP_HANDLE;
    //To use customized device event, app and driver should define same GUID.
    filter.dbch_eventguid = event_guid;
    //assign opened handle to specified device.
    //If service opened this device, driver of this device can send custom event by
    // IoReportTargetDeviceChange() or IoReportTargetDeviceChangeAsynchronous().
    filter.dbch_handle = device_handle;     

    HDEVNOTIFY handle = RegisterDeviceNotification(
        svc_handle,
        &filter,
        DEVICE_NOTIFY_SERVICE_HANDLE);
    return handle;
}
void UnregisterDeviceChangeEvents(HDEVNOTIFY handle)
{
    if(!UnregisterDeviceNotification(handle))
    {
        //write debuglog
    }
}
BOOL RegisterSessionChangeEvents(SERVICE_STATUS_HANDLE svc_handle)
{
    BOOL ok = WTSRegisterSessionNotification((HWND)svc_handle, NOTIFY_FOR_ALL_SESSIONS);
    if(!ok)
    {
        //write debuglog
    }
    return ok;
}
void UnregisterSessionChangeEvents(SERVICE_STATUS_HANDLE svc_handle)
{
    BOOL ok = WTSUnRegisterSessionNotification((HWND)svc_handle);
    if (!ok)
    {
        //write debuglog
    }
}
