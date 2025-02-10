#pragma once
DWORD HandleDeviceCustomEvent(DWORD event_type, PDEV_BROADCAST_HDR hdr);
DWORD HandleDeviceInterfaceEvent(DWORD event_type, PDEV_BROADCAST_HDR hdr);
DWORD HandleControlDeviceEvent(
    DWORD event_type,
    LPVOID event_data);

//Note: to use this way to send events, both driver and service should define same
//      GUID and event data structure.
// {1C23A568-DCCE-4596-BDF8-8FD3F3FA4950}
DEFINE_GUID(GUID_MY_CUSTOM_EVENT,
    0x1c23a568, 0xdcce, 0x4596, 0xbd, 0xf8, 0x8f, 0xd3, 0xf3, 0xfa, 0x49, 0x50);
typedef struct _MY_CUSTOM_EVENT {
    DWORD data;
}MY_CUSTOM_EVENT, * PMY_CUSTOM_EVENT;
