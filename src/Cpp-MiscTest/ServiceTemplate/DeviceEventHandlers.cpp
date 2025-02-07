#include "pch.h"

DWORD HandleDeviceCustomEvent(DWORD event_type, PDEV_BROADCAST_HDR hdr)
{
    //my customized device event arrived, it is sent from device 
    //which I opened a handle.
    //"dbh" stands for Device Broadcast Handle data
    PDEV_BROADCAST_HANDLE dbh = (PDEV_BROADCAST_HANDLE)hdr;
    PMY_CUSTOM_EVENT mydata = (PMY_CUSTOM_EVENT)dbh->dbch_data;

    //do something that you want to handle....
    UNREFERENCED_PARAMETER(dbh);
    UNREFERENCED_PARAMETER(mydata);

    return NO_ERROR;
}

DWORD HandleDeviceInterfaceEvent(DWORD event_type, PDEV_BROADCAST_HDR hdr)
{
    //"dbdev" stands for Device Broadcast DEViceinterface data
    PDEV_BROADCAST_DEVICEINTERFACE dbdev = (PDEV_BROADCAST_DEVICEINTERFACE)hdr;
    UNREFERENCED_PARAMETER(dbdev);
    switch (event_type)
    {
    case DBT_DEVICEARRIVAL: 
        //IRP_MJ_PNP + IRP_MN_START_DEVICE completed in specified device successfully.
        //=> in WDM, it indicates the callback AddDevice() done without any error.
        //=> in WDF, it indicates the callback EvtDriverDeviceAdd() done without any error.
        //Note: this event is fired by DeviceManager when DeviceInterface is ready to 
        //      retrieve requests.
        break;
    case DBT_DEVICEQUERYREMOVE:
        //IRP_MJ_PNP + IRP_MN_QUERY_REMOVE_DEVICE handled in specified device successfully.
        //IRP_MN_QUERY_REMOVE_DEVICE will be sent to driver BEFORE IRP_MN_REMOVE_DEVICE
        //to query "Is it ok to remove this device?"
        break;
    case DBT_DEVICEQUERYREMOVEFAILED:
        //IRP_MJ_PNP + IRP_MN_QUERY_REMOVE_DEVICE request failed in specified device.
        //IRP_MN_QUERY_REMOVE_DEVICE will be sent to driver BEFORE IRP_MN_REMOVE_DEVICE
        //to query "Is it ok to remove this device?"
        break;
    case DBT_DEVICEREMOVEPENDING:
        //IRP_MJ_PNP + IRP_MN_REMOVE_DEVICE completed in specified device, 
        //but device response PENDING.
        break;
    case DBT_DEVICEREMOVECOMPLETE:
        //IRP_MJ_PNP + IRP_MN_REMOVE_DEVICE completed in specified device successfully.
        break;
    }

    return NO_ERROR;
}
