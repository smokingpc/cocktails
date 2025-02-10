#pragma once

//ModernStandby is introduced since Win10.
//it comes from "ConnectedStandby" in Win8, and called "AOAC" in SoC platform(WoA).
BOOL IsModernStandbySupported();
HPOWERNOTIFY RegisterModernStandbyPowerEvents(SERVICE_STATUS_HANDLE svc_handle);
void UnregisterModernStandbyPowerEvents(HPOWERNOTIFY handle);
HDEVNOTIFY RegisterDeviceChangeEvents(
    SERVICE_STATUS_HANDLE svc_handle, 
    const GUID& class_guid);
HDEVNOTIFY RegisterDeviceCustomEvents(
    SERVICE_STATUS_HANDLE svc_handle, 
    const GUID& event_guid,
    HANDLE device_handle);
void UnregisterDeviceChangeEvents(HDEVNOTIFY handle);
BOOL RegisterSessionChangeEvents(SERVICE_STATUS_HANDLE svc_handle);
void UnregisterSessionChangeEvents(SERVICE_STATUS_HANDLE svc_handle);
